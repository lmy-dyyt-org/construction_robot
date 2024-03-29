/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-16     zhangjun     first version
 * 2021-02-20     lizhirui     fix warning
 * 2023-06-26     shell        clear ref to parent on waitpid()
 *                             Remove recycling of lwp on waitpid() and leave it to defunct routine
 * 2023-07-27     shell        Move the detach of children process on parent exit to lwp_terminate.
 *                             Make lwp_from_pid locked by caller to avoid possible use-after-free
 *                             error
 * 2023-10-27     shell        Format codes of sys_exit(). Fix the data racing where lock is missed
 *                             Add reference on pid/tid, so the resource is not freed while using.
 * 2024-01-25     shell        porting to new sched API
 */

/* includes scheduler related API */
#define __RT_IPC_SOURCE__

#include <rthw.h>
#include <rtthread.h>

#define DBG_TAG "lwp.pid"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <dfs_file.h>
#include <unistd.h>
#include <stdio.h> /* rename() */
#include <sys/stat.h>
#include <sys/statfs.h> /* statfs() */

#include "lwp_internal.h"
#include "tty.h"

#ifdef ARCH_MM_MMU
#include "lwp_user_mm.h"
#endif

#define PID_MAX 10000

#define PID_CT_ASSERT(name, x) \
    struct assert_##name {char ary[2 * (x) - 1];}

PID_CT_ASSERT(pid_min_nr, RT_LWP_MAX_NR > 1);
PID_CT_ASSERT(pid_max_nr, RT_LWP_MAX_NR < PID_MAX);

static struct lwp_avl_struct lwp_pid_ary[RT_LWP_MAX_NR];
static struct lwp_avl_struct *lwp_pid_free_head = RT_NULL;
static int lwp_pid_ary_alloced = 0;
static struct lwp_avl_struct *lwp_pid_root = RT_NULL;
static pid_t current_pid = 0;
static struct rt_mutex pid_mtx;

int lwp_pid_init(void)
{
    rt_mutex_init(&pid_mtx, "pidmtx", RT_IPC_FLAG_PRIO);
    return 0;
}

void lwp_pid_lock_take(void)
{
    LWP_DEF_RETURN_CODE(rc);

    rc = lwp_mutex_take_safe(&pid_mtx, RT_WAITING_FOREVER, 0);
    /* should never failed */
    RT_ASSERT(rc == RT_EOK);
}

void lwp_pid_lock_release(void)
{
    /* should never failed */
    if (lwp_mutex_release_safe(&pid_mtx) != RT_EOK)
        RT_ASSERT(0);
}

struct lwp_avl_struct *lwp_get_pid_ary(void)
{
    return lwp_pid_ary;
}

static pid_t lwp_pid_get_locked(void)
{
    struct lwp_avl_struct *p;
    pid_t pid = 0;

    p = lwp_pid_free_head;
    if (p)
    {
        lwp_pid_free_head = (struct lwp_avl_struct *)p->avl_right;
    }
    else if (lwp_pid_ary_alloced < RT_LWP_MAX_NR)
    {
        p = lwp_pid_ary + lwp_pid_ary_alloced;
        lwp_pid_ary_alloced++;
    }
    if (p)
    {
        int found_noused = 0;

        RT_ASSERT(p->data == RT_NULL);
        for (pid = current_pid + 1; pid < PID_MAX; pid++)
        {
            if (!lwp_avl_find(pid, lwp_pid_root))
            {
                found_noused = 1;
                break;
            }
        }
        if (!found_noused)
        {
            for (pid = 1; pid <= current_pid; pid++)
            {
                if (!lwp_avl_find(pid, lwp_pid_root))
                {
                    found_noused = 1;
                    break;
                }
            }
        }
        p->avl_key = pid;
        lwp_avl_insert(p, &lwp_pid_root);
        current_pid = pid;
    }
    return pid;
}

static void lwp_pid_put_locked(pid_t pid)
{
    struct lwp_avl_struct *p;

    if (pid == 0)
    {
        return;
    }

    p  = lwp_avl_find(pid, lwp_pid_root);
    if (p)
    {
        p->data = RT_NULL;
        lwp_avl_remove(p, &lwp_pid_root);
        p->avl_right = lwp_pid_free_head;
        lwp_pid_free_head = p;
    }
}

void lwp_pid_put(struct rt_lwp *lwp)
{
    lwp_pid_lock_take();
    lwp_pid_put_locked(lwp->pid);
    lwp_pid_lock_release();

    /* reset pid field */
    lwp->pid = 0;
    /* clear reference */
    lwp_ref_dec(lwp);
}

static void lwp_pid_set_lwp_locked(pid_t pid, struct rt_lwp *lwp)
{
    struct lwp_avl_struct *p;

    p  = lwp_avl_find(pid, lwp_pid_root);
    if (p)
    {
        p->data = lwp;
        lwp_ref_inc(lwp);
    }
}

static void __exit_files(struct rt_lwp *lwp)
{
    int fd = lwp->fdt.maxfd - 1;

    while (fd >= 0)
    {
        struct dfs_file *d;

        d = lwp->fdt.fds[fd];
        if (d)
        {
            dfs_file_close(d);
            fdt_fd_release(&lwp->fdt, fd);
        }
        fd--;
    }
}

void lwp_user_object_lock_init(struct rt_lwp *lwp)
{
    rt_mutex_init(&lwp->object_mutex, "lwp_obj", RT_IPC_FLAG_PRIO);
}

void lwp_user_object_lock_destroy(struct rt_lwp *lwp)
{
    rt_mutex_detach(&lwp->object_mutex);
}

void lwp_user_object_lock(struct rt_lwp *lwp)
{
    if (lwp)
    {
        rt_mutex_take(&lwp->object_mutex, RT_WAITING_FOREVER);
    }
    else
    {
        RT_ASSERT(0);
    }
}

void lwp_user_object_unlock(struct rt_lwp *lwp)
{
    if (lwp)
    {
        rt_mutex_release(&lwp->object_mutex);
    }
    else
    {
        RT_ASSERT(0);
    }
}

int lwp_user_object_add(struct rt_lwp *lwp, rt_object_t object)
{
    int ret = -1;

    if (lwp && object)
    {
        lwp_user_object_lock(lwp);
        if (!lwp_avl_find((avl_key_t)object, lwp->object_root))
        {
            struct lwp_avl_struct *node;

            node = (struct lwp_avl_struct *)rt_malloc(sizeof(struct lwp_avl_struct));
            if (node)
            {
                rt_atomic_add(&object->lwp_ref_count, 1);
                node->avl_key = (avl_key_t)object;
                lwp_avl_insert(node, &lwp->object_root);
                ret = 0;
            }
        }
        lwp_user_object_unlock(lwp);
    }
    return ret;
}

static rt_err_t _object_node_delete(struct rt_lwp *lwp, struct lwp_avl_struct *node)
{
    rt_err_t ret = -1;
    rt_object_t object;

    if (!lwp || !node)
    {
        return ret;
    }
    object = (rt_object_t)node->avl_key;
    object->lwp_ref_count--;
    if (object->lwp_ref_count == 0)
    {
        /* remove from kernel object list */
        switch (object->type)
        {
        case RT_Object_Class_Semaphore:
            ret = rt_sem_delete((rt_sem_t)object);
            break;
        case RT_Object_Class_Mutex:
            ret = rt_mutex_delete((rt_mutex_t)object);
            break;
        case RT_Object_Class_Event:
            ret = rt_event_delete((rt_event_t)object);
            break;
        case RT_Object_Class_MailBox:
            ret = rt_mb_delete((rt_mailbox_t)object);
            break;
        case RT_Object_Class_MessageQueue:
            ret = rt_mq_delete((rt_mq_t)object);
            break;
        case RT_Object_Class_Timer:
            ret = rt_timer_delete((rt_timer_t)object);
            break;
        case RT_Object_Class_Custom:
            ret = rt_custom_object_destroy(object);
            break;
        default:
            LOG_E("input object type(%d) error", object->type);
            break;
        }
    }
    else
    {
        ret = 0;
    }
    lwp_avl_remove(node, &lwp->object_root);
    rt_free(node);
    return ret;
}

rt_err_t lwp_user_object_delete(struct rt_lwp *lwp, rt_object_t object)
{
    rt_err_t ret = -1;

    if (lwp && object)
    {
        struct lwp_avl_struct *node;

        lwp_user_object_lock(lwp);
        node = lwp_avl_find((avl_key_t)object, lwp->object_root);
        ret = _object_node_delete(lwp, node);
        lwp_user_object_unlock(lwp);
    }
    return ret;
}

void lwp_user_object_clear(struct rt_lwp *lwp)
{
    struct lwp_avl_struct *node;

    lwp_user_object_lock(lwp);
    while ((node = lwp_map_find_first(lwp->object_root)) != RT_NULL)
    {
        _object_node_delete(lwp, node);
    }
    lwp_user_object_unlock(lwp);
}

static int _object_dup(struct lwp_avl_struct *node, void *arg)
{
    rt_object_t object;
    struct rt_lwp *dst_lwp = (struct rt_lwp *)arg;

    object = (rt_object_t)node->avl_key;
    lwp_user_object_add(dst_lwp, object);
    return 0;
}

void lwp_user_object_dup(struct rt_lwp *dst_lwp, struct rt_lwp *src_lwp)
{
    lwp_user_object_lock(src_lwp);
    lwp_avl_traversal(src_lwp->object_root, _object_dup, dst_lwp);
    lwp_user_object_unlock(src_lwp);
}

rt_lwp_t lwp_create(rt_base_t flags)
{
    pid_t pid;
    rt_lwp_t new_lwp = rt_calloc(1, sizeof(struct rt_lwp));

    if (new_lwp)
    {
        /* minimal setup of lwp object */
        new_lwp->session = -1;
        new_lwp->ref = 1;
#ifdef RT_USING_SMP
        new_lwp->bind_cpu = RT_CPUS_NR;
#endif
        rt_list_init(&new_lwp->wait_list);
        rt_list_init(&new_lwp->t_grp);
        rt_list_init(&new_lwp->timer);
        lwp_user_object_lock_init(new_lwp);
        rt_wqueue_init(&new_lwp->wait_queue);
        lwp_signal_init(&new_lwp->signal);
        rt_mutex_init(&new_lwp->lwp_lock, "lwp_lock", RT_IPC_FLAG_PRIO);

        /* lwp with pid */
        if (flags & LWP_CREATE_FLAG_ALLOC_PID)
        {
            lwp_pid_lock_take();
            pid = lwp_pid_get_locked();
            if (pid == 0)
            {
                lwp_user_object_lock_destroy(new_lwp);
                rt_free(new_lwp);
                new_lwp = RT_NULL;
                LOG_E("pid slot fulled!\n");
            }
            else
            {
                new_lwp->pid = pid;
                lwp_pid_set_lwp_locked(pid, new_lwp);
            }
            lwp_pid_lock_release();
        }
    }

    LOG_D("%s(pid=%d) => %p", __func__, new_lwp ? new_lwp->pid : -1, new_lwp);
    return new_lwp;
}

/** when reference is 0, a lwp can be released */
void lwp_free(struct rt_lwp* lwp)
{
    if (lwp == RT_NULL)
    {
        return;
    }

    /**
     * Brief: Recycle the lwp when reference is cleared
     *
     * Note: Critical Section
     * - lwp (RW. there is no other writer/reader compete with lwp_free, since
     *   all the reference is clear)
     */
    LOG_D("lwp free: %p", lwp);

    LWP_LOCK(lwp);

    if (lwp->args != RT_NULL)
    {
#ifndef ARCH_MM_MMU
        lwp->args_length = RT_NULL;
#ifndef ARCH_MM_MPU
        rt_free(lwp->args);
#endif /* not defined ARCH_MM_MPU */
#endif /* ARCH_MM_MMU */
        lwp->args = RT_NULL;
    }

    lwp_user_object_clear(lwp);
    lwp_user_object_lock_destroy(lwp);

    /* free data section */
    if (lwp->data_entry != RT_NULL)
    {
#ifdef ARCH_MM_MMU
        rt_free_align(lwp->data_entry);
#else
#ifdef ARCH_MM_MPU
        rt_lwp_umap_user(lwp, lwp->text_entry, 0);
        rt_lwp_free_user(lwp, lwp->data_entry, lwp->data_size);
#else
        rt_free_align(lwp->data_entry);
#endif /* ARCH_MM_MPU */
#endif /* ARCH_MM_MMU */
        lwp->data_entry = RT_NULL;
    }

    /* free text section */
    if (lwp->lwp_type == LWP_TYPE_DYN_ADDR)
    {
        if (lwp->text_entry)
        {
            LOG_D("lwp text free: %p", lwp->text_entry);
#ifndef ARCH_MM_MMU
            rt_free((void*)lwp->text_entry);
#endif /* not defined ARCH_MM_MMU */
            lwp->text_entry = RT_NULL;
        }
    }

#ifdef ARCH_MM_MMU
    lwp_unmap_user_space(lwp);
#endif
    timer_list_free(&lwp->timer);

    LWP_UNLOCK(lwp);
    RT_ASSERT(lwp->lwp_lock.owner == RT_NULL);
    rt_mutex_detach(&lwp->lwp_lock);

    /**
     * pid must have release before enter lwp_free()
     * otherwise this is a data racing
     */
    RT_ASSERT(lwp->pid == 0);
    rt_free(lwp);
}

rt_inline rt_noreturn
void _thread_exit(rt_lwp_t lwp, rt_thread_t thread)
{
    /**
     * Note: the tid tree always hold a reference to thread, hence the tid must
     * be release before cleanup of thread
     */
    lwp_tid_put(thread->tid);
    thread->tid = 0;

    LWP_LOCK(lwp);
    rt_list_remove(&thread->sibling);
    LWP_UNLOCK(lwp);

    rt_thread_delete(thread);
    rt_schedule();
    while (1) ;
}

rt_inline void _clear_child_tid(rt_thread_t thread)
{
    if (thread->clear_child_tid)
    {
        int t = 0;
        int *clear_child_tid = thread->clear_child_tid;

        thread->clear_child_tid = RT_NULL;
        lwp_put_to_user(clear_child_tid, &t, sizeof t);
        sys_futex(clear_child_tid, FUTEX_WAKE | FUTEX_PRIVATE, 1, RT_NULL, RT_NULL, 0);
    }
}

void lwp_exit(rt_lwp_t lwp, rt_base_t status)
{
    rt_thread_t thread;

    if (!lwp)
    {
        LOG_W("%s: lwp should not be null", __func__);
        return ;
    }

    thread = rt_thread_self();
    RT_ASSERT((struct rt_lwp *)thread->lwp == lwp);
    LOG_D("process(lwp.pid=%d) exit", lwp->pid);

#ifdef ARCH_MM_MMU
    _clear_child_tid(thread);

    LWP_LOCK(lwp);
    /**
     * Brief: only one thread should calls exit_group(),
     * but we can not ensured that during run-time
     */
    lwp->lwp_ret = LWP_CREATE_STAT(status);
    LWP_UNLOCK(lwp);

    lwp_terminate(lwp);
#else
    main_thread = rt_list_entry(lwp->t_grp.prev, struct rt_thread, sibling);
    if (main_thread == tid)
    {
        rt_thread_t sub_thread;
        rt_list_t *list;

        lwp_terminate(lwp);

        /* delete all subthread */
        while ((list = tid->sibling.prev) != &lwp->t_grp)
        {
            sub_thread = rt_list_entry(list, struct rt_thread, sibling);
            rt_list_remove(&sub_thread->sibling);
            rt_thread_delete(sub_thread);
        }
        lwp->lwp_ret = value;
    }
#endif /* ARCH_MM_MMU */

    _thread_exit(lwp, thread);
}

void lwp_thread_exit(rt_thread_t thread, rt_base_t status)
{
    rt_thread_t header_thr;
    struct rt_lwp *lwp;

    LOG_D("%s", __func__);

    RT_ASSERT(thread == rt_thread_self());
    lwp = (struct rt_lwp *)thread->lwp;
    RT_ASSERT(lwp != RT_NULL);

#ifdef ARCH_MM_MMU
    _clear_child_tid(thread);

    LWP_LOCK(lwp);
    header_thr = rt_list_entry(lwp->t_grp.prev, struct rt_thread, sibling);
    if (header_thr == thread && thread->sibling.prev == &lwp->t_grp)
    {
        lwp->lwp_ret = LWP_CREATE_STAT(status);
        LWP_UNLOCK(lwp);

        lwp_terminate(lwp);
    }
    else
    {
        LWP_UNLOCK(lwp);
    }
#endif /* ARCH_MM_MMU */

    _thread_exit(lwp, thread);
}

static void _pop_tty(rt_lwp_t lwp)
{
    if (!lwp->background)
    {
        struct termios *old_stdin_termios = get_old_termios();
        struct rt_lwp *old_lwp = NULL;

        if (lwp->session == -1)
        {
            tcsetattr(1, 0, old_stdin_termios);
        }
        if (lwp->tty != RT_NULL)
        {
            rt_mutex_take(&lwp->tty->lock, RT_WAITING_FOREVER);
            if (lwp->tty->foreground == lwp)
            {
                old_lwp = tty_pop(&lwp->tty->head, RT_NULL);
                lwp->tty->foreground = old_lwp;
            }
            else
            {
                tty_pop(&lwp->tty->head, lwp);
            }
            rt_mutex_release(&lwp->tty->lock);

            LWP_LOCK(lwp);
            lwp->tty = RT_NULL;
            LWP_UNLOCK(lwp);
        }
    }
}

/** @note the reference is not for synchronization, but for the release of resource. the synchronization is done through lwp & pid lock */
int lwp_ref_inc(struct rt_lwp *lwp)
{
    int ref;
    ref = rt_atomic_add(&lwp->ref, 1);
    LOG_D("%s(%p(%s)): before %d", __func__, lwp, lwp->cmd, ref);

    return ref;
}

int lwp_ref_dec(struct rt_lwp *lwp)
{
    int ref;

    ref = rt_atomic_add(&lwp->ref, -1);
    LOG_D("%s(lwp=%p,lwp->cmd=%s): before ref=%d", __func__, lwp, lwp->cmd, ref);

    if (ref == 1)
    {
        struct rt_channel_msg msg;

        if (lwp->debug)
        {
            memset(&msg, 0, sizeof msg);
            rt_raw_channel_send(gdb_server_channel(), &msg);
        }

#ifndef ARCH_MM_MMU
#ifdef RT_LWP_USING_SHM
        lwp_shm_lwp_free(lwp);
#endif /* RT_LWP_USING_SHM */
#endif /* not defined ARCH_MM_MMU */
        lwp_free(lwp);
    }
    else
    {
        /* reference must be a positive integer */
        RT_ASSERT(ref > 1);
    }

    return ref;
}

struct rt_lwp* lwp_from_pid_locked(pid_t pid)
{
    struct lwp_avl_struct *p;
    struct rt_lwp *lwp = RT_NULL;

    p  = lwp_avl_find(pid, lwp_pid_root);
    if (p)
    {
        lwp = (struct rt_lwp *)p->data;
    }

    return lwp;
}

pid_t lwp_to_pid(struct rt_lwp* lwp)
{
    if (!lwp)
    {
        return 0;
    }
    return lwp->pid;
}

char* lwp_pid2name(int32_t pid)
{
    struct rt_lwp *lwp;
    char* process_name = RT_NULL;

    lwp_pid_lock_take();
    lwp = lwp_from_pid_locked(pid);
    if (lwp)
    {
        process_name = strrchr(lwp->cmd, '/');
        process_name = process_name? process_name + 1: lwp->cmd;
    }
    lwp_pid_lock_release();

    return process_name;
}

pid_t lwp_name2pid(const char *name)
{
    int idx;
    pid_t pid = 0;
    rt_thread_t main_thread;
    char* process_name = RT_NULL;
    rt_sched_lock_level_t slvl;

    lwp_pid_lock_take();
    for (idx = 0; idx < RT_LWP_MAX_NR; idx++)
    {
        /* 0 is reserved */
        struct rt_lwp *lwp = (struct rt_lwp *)lwp_pid_ary[idx].data;

        if (lwp)
        {
            process_name = strrchr(lwp->cmd, '/');
            process_name = process_name? process_name + 1: lwp->cmd;
            if (!rt_strncmp(name, process_name, RT_NAME_MAX))
            {
                main_thread = rt_list_entry(lwp->t_grp.prev, struct rt_thread, sibling);
                rt_sched_lock(&slvl);
                if (!(rt_sched_thread_get_stat(main_thread) == RT_THREAD_CLOSE))
                {
                    pid = lwp->pid;
                }
                rt_sched_unlock(slvl);
            }
        }
    }
    lwp_pid_lock_release();
    return pid;
}

int lwp_getpid(void)
{
    return ((struct rt_lwp *)rt_thread_self()->lwp)->pid;
}

/**
 * @brief Wait for a child lwp to terminate. Do the essential recycling. Setup
 *        status code for user
 */
static sysret_t _lwp_wait_and_recycle(struct rt_lwp *child, rt_thread_t cur_thr,
                                      struct rt_lwp *self_lwp, int *status,
                                      int options)
{
    sysret_t error;
    int lwp_stat;
    int terminated;

    if (!child)
    {
        error = -RT_ERROR;
    }
    else
    {
        /**
         * Note: Critical Section
         * - child lwp (RW. This will modify its parent if valid)
         */
        LWP_LOCK(child);
        if (child->terminated)
        {
            error = child->pid;
        }
        else if (rt_list_isempty(&child->wait_list))
        {
            /**
             * Note: only one thread can wait on wait_list.
             * dont reschedule before mutex unlock
             */
            rt_enter_critical();

            error = rt_thread_suspend_with_flag(cur_thr, RT_INTERRUPTIBLE);
            if (error == 0)
            {
                rt_list_insert_before(&child->wait_list, &RT_THREAD_LIST_NODE(cur_thr));
                LWP_UNLOCK(child);

                rt_set_errno(RT_EINTR);
                rt_exit_critical();
                rt_schedule();

                /**
                 * Since parent is holding a reference to children this lock will
                 * not be freed before parent dereference to it.
                 */
                LWP_LOCK(child);

                error = rt_get_errno();
                if (error == RT_EINTR)
                {
                    error = -EINTR;
                }
                else if (error != RT_EOK)
                {
                    LOG_W("%s: unexpected error code %ld", __func__, error);
                }
                else
                {
                    error = child->pid;
                }
            }
            else
                rt_exit_critical();
        }
        else
            error = -RT_EINTR;

        lwp_stat = child->lwp_ret;
        terminated = child->terminated;
        LWP_UNLOCK(child);

        if (error > 0)
        {
            if (terminated)
            {
                LOG_D("func %s: child detached", __func__);
                /** Reap the child process if it's exited */
                lwp_pid_put(child);
                lwp_children_unregister(self_lwp, child);
            }

            if (status)
                lwp_data_put(self_lwp, status, &lwp_stat, sizeof(*status));
        }
    }

    return error;
}

pid_t waitpid(pid_t pid, int *status, int options) __attribute__((alias("lwp_waitpid")));

pid_t lwp_waitpid(const pid_t pid, int *status, int options)
{
    pid_t rc = -1;
    struct rt_thread *thread;
    struct rt_lwp *child;
    struct rt_lwp *self_lwp;

    thread = rt_thread_self();
    self_lwp = lwp_self();

    if (!self_lwp)
    {
        rc = -RT_EINVAL;
    }
    else
    {
        if (pid > 0)
        {
            lwp_pid_lock_take();
            child = lwp_from_pid_locked(pid);
            if (child->parent != self_lwp)
                rc = -RT_ERROR;
            else
                rc = RT_EOK;
            lwp_pid_lock_release();

            if (rc == RT_EOK)
                rc = _lwp_wait_and_recycle(child, thread, self_lwp, status, options);
        }
        else if (pid == -1)
        {
            LWP_LOCK(self_lwp);
            child = self_lwp->first_child;
            LWP_UNLOCK(self_lwp);
            RT_ASSERT(!child || child->parent == self_lwp);

            rc = _lwp_wait_and_recycle(child, thread, self_lwp, status, options);
        }
        else
        {
            /* not supported yet */
            rc = -RT_EINVAL;
        }
    }

    if (rc > 0)
    {
        LOG_D("%s: recycle child id %ld (status=0x%x)", __func__, (long)rc, status ? *status : 0);
    }
    else
    {
        RT_ASSERT(rc != 0);
        LOG_D("%s: wait failed with code %ld", __func__, (long)rc);
    }

    return rc;
}

#ifdef RT_USING_FINSH
/* copy from components/finsh/cmd.c */
static void object_split(int len)
{
    while (len--)
    {
        rt_kprintf("-");
    }
}

static void print_thread_info(struct rt_thread* thread, int maxlen)
{
    rt_uint8_t *ptr;
    rt_uint8_t stat;

#ifdef RT_USING_SMP
    if (RT_SCHED_CTX(thread).oncpu != RT_CPU_DETACHED)
        rt_kprintf("%-*.*s %3d %3d ", maxlen, RT_NAME_MAX, thread->parent.name, RT_SCHED_CTX(thread).oncpu, RT_SCHED_PRIV(thread).current_priority);
    else
        rt_kprintf("%-*.*s N/A %3d ", maxlen, RT_NAME_MAX, thread->parent.name, RT_SCHED_PRIV(thread).current_priority);
#else
    rt_kprintf("%-*.*s %3d ", maxlen, RT_NAME_MAX, thread->parent.name, thread->current_priority);
#endif /*RT_USING_SMP*/

    stat = (RT_SCHED_CTX(thread).stat & RT_THREAD_STAT_MASK);
    if (stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
    else if ((stat & RT_THREAD_SUSPEND_MASK) == RT_THREAD_SUSPEND_MASK) rt_kprintf(" suspend");
    else if (stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
    else if (stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");
    else if (stat == RT_THREAD_RUNNING) rt_kprintf(" running");

#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
    ptr = (rt_uint8_t *)thread->stack_addr + thread->stack_size;
    while (*ptr == '#')ptr--;

    rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
            ((rt_uint32_t)thread->sp - (rt_uint32_t)thread->stack_addr),
            thread->stack_size,
            ((rt_uint32_t)ptr - (rt_uint32_t)thread->stack_addr) * 100 / thread->stack_size,
            thread->remaining_tick,
            thread->error);
#else
    ptr = (rt_uint8_t *)thread->stack_addr;
    while (*ptr == '#')ptr++;

    rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
            (thread->stack_size + (rt_uint32_t)(rt_size_t)thread->stack_addr - (rt_uint32_t)(rt_size_t)thread->sp),
            thread->stack_size,
            (thread->stack_size + (rt_uint32_t)(rt_size_t)thread->stack_addr - (rt_uint32_t)(rt_size_t)ptr) * 100
            / thread->stack_size,
            RT_SCHED_PRIV(thread).remaining_tick,
            thread->error);
#endif
}

long list_process(void)
{
    int index;
    int maxlen;
    rt_ubase_t level;
    struct rt_thread *thread;
    struct rt_list_node *node, *list;
    const char *item_title = "thread";

    int count = 0;
    struct rt_thread **threads;

    maxlen = RT_NAME_MAX;
#ifdef RT_USING_SMP
    rt_kprintf("%-*.s %-*.s %-*.s cpu pri  status      sp     stack size max used left tick  error\n", 4, "PID", maxlen, "CMD", maxlen, item_title);
    object_split(4);rt_kprintf(" ");object_split(maxlen);rt_kprintf(" ");object_split(maxlen);rt_kprintf(" ");
    rt_kprintf(                  "--- ---  ------- ---------- ----------  ------  ---------- ---\n");
#else
    rt_kprintf("%-*.s %-*.s %-*.s pri  status      sp     stack size max used left tick  error\n", 4, "PID", maxlen, "CMD", maxlen, item_title);
    object_split(4);rt_kprintf(" ");object_split(maxlen);rt_kprintf(" ");object_split(maxlen);rt_kprintf(" ");
    rt_kprintf(                  "---  ------- ---------- ----------  ------  ---------- ---\n");
#endif /*RT_USING_SMP*/

    count = rt_object_get_length(RT_Object_Class_Thread);
    if (count > 0)
    {
        /* get thread pointers */
        threads = (struct rt_thread **)rt_calloc(count, sizeof(struct rt_thread *));
        if (threads)
        {
            index = rt_object_get_pointers(RT_Object_Class_Thread, (rt_object_t *)threads, count);

            if (index > 0)
            {
                for (index = 0; index <count; index++)
                {
                    struct rt_thread th;

                    thread = threads[index];

                    level = rt_spin_lock_irqsave(&thread->spinlock);
                    if ((rt_object_get_type(&thread->parent) & ~RT_Object_Class_Static) != RT_Object_Class_Thread)
                    {
                        rt_spin_unlock_irqrestore(&thread->spinlock, level);
                        continue;
                    }

                    rt_memcpy(&th, thread, sizeof(struct rt_thread));
                    rt_spin_unlock_irqrestore(&thread->spinlock, level);

                    if (th.lwp == RT_NULL)
                    {
                        rt_kprintf("     %-*.*s ", maxlen, RT_NAME_MAX, "kernel");
                        print_thread_info(&th, maxlen);
                    }
                }
            }
            rt_free(threads);
        }
    }

    for (index = 0; index < RT_LWP_MAX_NR; index++)
    {
        struct rt_lwp *lwp = (struct rt_lwp *)lwp_pid_ary[index].data;

        if (lwp)
        {
            list = &lwp->t_grp;
            for (node = list->next; node != list; node = node->next)
            {
                thread = rt_list_entry(node, struct rt_thread, sibling);
                rt_kprintf("%4d %-*.*s ", lwp_to_pid(lwp), maxlen, RT_NAME_MAX, lwp->cmd);
                print_thread_info(thread, maxlen);
            }
        }
    }
    return 0;
}
MSH_CMD_EXPORT(list_process, list process);

static void cmd_kill(int argc, char** argv)
{
    int pid;
    int sig = SIGKILL;

    if (argc < 2)
    {
        rt_kprintf("kill pid or kill pid -s signal\n");
        return;
    }

    pid = atoi(argv[1]);
    if (argc >= 4)
    {
        if (argv[2][0] == '-' && argv[2][1] == 's')
        {
            sig = atoi(argv[3]);
        }
    }
    lwp_pid_lock_take();
    lwp_signal_kill(lwp_from_pid_locked(pid), sig, SI_USER, 0);
    lwp_pid_lock_release();
}
MSH_CMD_EXPORT_ALIAS(cmd_kill, kill, send a signal to a process);

static void cmd_killall(int argc, char** argv)
{
    int pid;
    if (argc < 2)
    {
        rt_kprintf("killall processes_name\n");
        return;
    }

    while((pid = lwp_name2pid(argv[1])) > 0)
    {
        lwp_pid_lock_take();
        lwp_signal_kill(lwp_from_pid_locked(pid), SIGKILL, SI_USER, 0);
        lwp_pid_lock_release();
        rt_thread_mdelay(100);
    }
}
MSH_CMD_EXPORT_ALIAS(cmd_killall, killall, kill processes by name);

#endif

int lwp_check_exit_request(void)
{
    rt_thread_t thread = rt_thread_self();
    rt_base_t expected = LWP_EXIT_REQUEST_TRIGGERED;

    if (!thread->lwp)
    {
        return 0;
    }

    return rt_atomic_compare_exchange_strong(&thread->exit_request, &expected,
                                             LWP_EXIT_REQUEST_IN_PROCESS);
}

static void _wait_sibling_exit(rt_lwp_t lwp, rt_thread_t curr_thread);
static void _resr_cleanup(struct rt_lwp *lwp);

void lwp_terminate(struct rt_lwp *lwp)
{
    if (!lwp)
    {
        /* kernel thread not support */
        return;
    }

    LOG_D("%s(lwp=%p \"%s\")", __func__, lwp, lwp->cmd);

    LWP_LOCK(lwp);

    if (!lwp->terminated)
    {
        /* stop the receiving of signals */
        lwp->terminated = RT_TRUE;
        LWP_UNLOCK(lwp);

        _wait_sibling_exit(lwp, rt_thread_self());
        _resr_cleanup(lwp);
    }
    else
    {
        LWP_UNLOCK(lwp);
    }
}

static void _wait_sibling_exit(rt_lwp_t lwp, rt_thread_t curr_thread)
{
    rt_sched_lock_level_t slvl;
    rt_list_t *list;
    rt_thread_t thread;
    rt_base_t expected = LWP_EXIT_REQUEST_NONE;

    /* broadcast exit request for sibling threads */
    LWP_LOCK(lwp);
    for (list = lwp->t_grp.next; list != &lwp->t_grp; list = list->next)
    {
        thread = rt_list_entry(list, struct rt_thread, sibling);

        rt_atomic_compare_exchange_strong(&thread->exit_request, &expected,
                                          LWP_EXIT_REQUEST_TRIGGERED);

        rt_sched_lock(&slvl);
        /* dont release, otherwise thread may have been freed */
        if (rt_sched_thread_is_suspended(thread))
        {
            thread->error = RT_EINTR;
            rt_sched_unlock(slvl);

            rt_thread_wakeup(thread);
        }
        else
        {
            rt_sched_unlock(slvl);
        }
    }
    LWP_UNLOCK(lwp);

    while (1)
    {
        int subthread_is_terminated;
        LOG_D("%s: wait for subthread exiting", __func__);

        /**
         * Brief: wait for all *running* sibling threads to exit
         *
         * Note: Critical Section
         * - sibling list of lwp (RW. It will clear all siblings finally)
         */
        LWP_LOCK(lwp);
        subthread_is_terminated = (int)(curr_thread->sibling.prev == &lwp->t_grp);
        if (!subthread_is_terminated)
        {
            rt_sched_lock_level_t slvl;
            rt_thread_t sub_thread;
            rt_list_t *list;
            int all_subthread_in_init = 1;

            /* check all subthread is in init state */
            for (list = curr_thread->sibling.prev; list != &lwp->t_grp; list = list->prev)
            {
                rt_sched_lock(&slvl);
                sub_thread = rt_list_entry(list, struct rt_thread, sibling);
                if (rt_sched_thread_get_stat(sub_thread) != RT_THREAD_INIT)
                {
                    rt_sched_unlock(slvl);
                    all_subthread_in_init = 0;
                    break;
                }
                else
                {
                    rt_sched_unlock(slvl);
                }
            }
            if (all_subthread_in_init)
            {
                /* delete all subthread */
                while ((list = curr_thread->sibling.prev) != &lwp->t_grp)
                {
                    sub_thread = rt_list_entry(list, struct rt_thread, sibling);
                    rt_list_remove(&sub_thread->sibling);

                    /**
                     * Note: Critical Section
                     * - thread control block (RW. Since it will free the thread
                     *   control block, it must ensure no one else can access
                     *   thread any more)
                     */
                    lwp_tid_put(sub_thread->tid);
                    sub_thread->tid = 0;
                    rt_thread_delete(sub_thread);
                }
                subthread_is_terminated = 1;
            }
        }
        LWP_UNLOCK(lwp);

        if (subthread_is_terminated)
        {
            break;
        }
        rt_thread_mdelay(10);
    }
}

static void _resr_cleanup(struct rt_lwp *lwp)
{
    LWP_LOCK(lwp);
    lwp_signal_detach(&lwp->signal);

    /**
     * @brief Detach children from lwp
     *
     * @note Critical Section
     * - the lwp (RW. Release lwp)
     * - the pid resource manager (RW. Release the pid)
     */
    while (lwp->first_child)
    {
        struct rt_lwp *child;

        child = lwp->first_child;
        lwp->first_child = child->sibling;

        /** @note safe since the slist node is release */
        LWP_UNLOCK(lwp);
        LWP_LOCK(child);
        if (child->terminated)
        {
            lwp_pid_put(child);
        }
        else
        {
            child->sibling = RT_NULL;
            /* info: this may cause an orphan lwp */
            child->parent = RT_NULL;
        }

        LWP_UNLOCK(child);
        lwp_ref_dec(child);
        lwp_ref_dec(lwp);

        LWP_LOCK(lwp);
    }
    LWP_UNLOCK(lwp);

    _pop_tty(lwp);

    /**
     * @brief Wakeup parent if it's waiting for this lwp, otherwise a signal
     *        will be sent to parent
     *
     * @note Critical Section
     * - the parent lwp (RW.)
     */
    LWP_LOCK(lwp);
    if (lwp->parent)
    {
        struct rt_thread *thread;

        LWP_UNLOCK(lwp);
        if (!rt_list_isempty(&lwp->wait_list))
        {
            thread = RT_THREAD_LIST_NODE_ENTRY(lwp->wait_list.next);
            thread->error = RT_EOK;
            thread->msg_ret = (void*)(rt_size_t)lwp->lwp_ret;
            rt_thread_resume(thread);
        }
        else
        {
            /* children cannot detach itself and must wait for parent to take care of it */
            lwp_signal_kill(lwp->parent, SIGCHLD, CLD_EXITED, 0);
        }
    }
    else
    {
        LWP_UNLOCK(lwp);

        /* INFO: orphan hasn't parents to do the reap of pid */
        lwp_pid_put(lwp);
    }

    LWP_LOCK(lwp);
    if (lwp->fdt.fds != RT_NULL)
    {
        struct dfs_file **fds;

        /* auto clean fds */
        __exit_files(lwp);
        fds = lwp->fdt.fds;
        lwp->fdt.fds = RT_NULL;
        LWP_UNLOCK(lwp);

        rt_free(fds);
    }
    else
    {
        LWP_UNLOCK(lwp);
    }
}

static int _lwp_setaffinity(pid_t pid, int cpu)
{
    struct rt_lwp *lwp;
    int ret = -1;

    lwp_pid_lock_take();
    if(pid == 0)
        lwp = lwp_self();
    else
        lwp = lwp_from_pid_locked(pid);
    if (lwp)
    {
#ifdef RT_USING_SMP
        rt_list_t *list;

        lwp->bind_cpu = cpu;
        for (list = lwp->t_grp.next; list != &lwp->t_grp; list = list->next)
        {
            rt_thread_t thread;

            thread = rt_list_entry(list, struct rt_thread, sibling);
            rt_thread_control(thread, RT_THREAD_CTRL_BIND_CPU, (void *)(rt_size_t)cpu);
        }
#endif
        ret = 0;
    }
    lwp_pid_lock_release();
    return ret;
}

int lwp_setaffinity(pid_t pid, int cpu)
{
    int ret;

#ifdef RT_USING_SMP
    if (cpu < 0 || cpu > RT_CPUS_NR)
    {
        cpu = RT_CPUS_NR;
    }
#endif
    ret = _lwp_setaffinity(pid, cpu);
    return ret;
}

#ifdef RT_USING_SMP
static void cmd_cpu_bind(int argc, char** argv)
{
    int pid;
    int cpu;

    if (argc < 3)
    {
        rt_kprintf("Useage: cpu_bind pid cpu\n");
        return;
    }

    pid = atoi(argv[1]);
    cpu = atoi(argv[2]);
    lwp_setaffinity((pid_t)pid, cpu);
}
MSH_CMD_EXPORT_ALIAS(cmd_cpu_bind, cpu_bind, set a process bind to a cpu);
#endif
