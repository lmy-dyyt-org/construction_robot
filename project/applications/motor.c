/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-03 15:24:57
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-17 09:19:59
 * @FilePath: \project\applications\motor.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file motor.c
 * @brief Motor control functions
 * @version 1.0
 * @date 2021-04-21
 */

#include "motor.h"
#include "ulog.h"
#include "finsh.h"
#include "ulog.h"

static int motor_id = 0; // 用于分配电机id

#define MOTOR_ASSERT(x) \
    if (!(x))           \
        while (1)       \
            ;
                         // 电机操作函数的
#define MOTOR_ASSERT_ID(id) \
    if (id >= MOTOR_NUM)    \
        return 0;
#define MOTOR_MALLOC(x) malloc(x); // 电机操作函数的内存分配
#define MOTOR_FREE(x) free(x);     // 电机操作函数的内存释放

enum
{
    M_EOK = 0U,
    M_EFAIL,
    M_EINVAL,
    M_EBUSY,
    M_ENOMEM,
};

extern motor_t motor_list[MOTOR_NUM];

/**
 * @brief 获取电机
 *
 * @param id 电机id
 * @return motor_t* 返回电机指针
 */
inline motor_t *motor_get(int id)
{
    MOTOR_ASSERT_ID(id);
    return &motor_list[id];
}

// 底层不支持
int motor_behiver_1(int id, uint16_t mode, void *data, void *user_data)
{
    motor_t *motor = motor_get(id);
    apid_t *pid_torque = motor->pid_torque;
    apid_t *pid_speed = motor->pid_speed;
    apid_t *pid_pos = motor->pid_pos;
    PID_TYPE cycle = *((PID_TYPE *)(data));
    PID_TYPE tmpout = 0;
    switch (mode)
    {

    case MOTOR_MODE_POS:
    {
        APID_Set_Target(pid_pos, motor->tar_pos);
        APID_Set_Present(pid_pos, motor->cur_pos);
        APID_Hander(pid_pos, cycle);
        motor->tar_speed = APID_Get_Out(pid_pos);
    }
    case MOTOR_MODE_SPEED:
    {
        APID_Set_Target(pid_speed, motor->tar_speed);
        APID_Set_Present(pid_speed, motor->cur_speed);
        APID_Hander(pid_speed, cycle);
        motor->tar_torque = APID_Get_Out(pid_speed);
    }
    case MOTOR_MODE_TORQUE:
    {
        APID_Set_Target(pid_torque, motor->tar_torque);
        APID_Set_Present(pid_torque, motor->cur_torque);
        APID_Hander(pid_torque, cycle);
        motor->acc_out = APID_Get_Out(pid_torque);
        break;
    }

    default:
    {
        break;
    }
    }
    return 0;
}
// 底层支持torque
int motor_behiver_2(int id, uint16_t mode, void *data, void *user_data)
{
    motor_t *motor = motor_get(id);
    apid_t *pid_torque = motor->pid_torque;
    apid_t *pid_speed = motor->pid_speed;
    apid_t *pid_pos = motor->pid_pos;
    PID_TYPE cycle = *((PID_TYPE *)(data));
    PID_TYPE tmpout = 0;
    switch (mode)
    {

    case MOTOR_MODE_POS:
    {
        APID_Set_Target(pid_pos, motor->tar_pos);
        APID_Set_Present(pid_pos, motor->cur_pos);
        APID_Hander(pid_pos, cycle);
        motor->tar_speed = APID_Get_Out(pid_pos);
    }
    case MOTOR_MODE_SPEED:
    {
        APID_Set_Target(pid_speed, motor->tar_speed);
        APID_Set_Present(pid_speed, motor->cur_speed);
        APID_Hander(pid_speed, cycle);
        motor->tar_torque = APID_Get_Out(pid_speed);
        // LOG_RAW("name%d speed t p o:%f,%f,%f\r\n", motor->name, pid_speed->parameter.target, pid_speed->parameter.present, pid_speed->parameter.out);
        ////////////////////////////////
    }
    case MOTOR_MODE_TORQUE:
    {
        motor->acc_out = motor->tar_torque;

        break;
    }

    default:
    {
        break;
    }
    }
    return 0;
}
// 底层支持torque,speed
int motor_behiver_3(int id, uint16_t mode, void *data, void *user_data)
{
    motor_t *motor = motor_get(id);
    apid_t *pid_torque = motor->pid_torque;
    apid_t *pid_speed = motor->pid_speed;
    apid_t *pid_pos = motor->pid_pos;
    PID_TYPE cycle = *((PID_TYPE *)(data));
    PID_TYPE tmpout = 0;
    switch (mode)
    {

    case MOTOR_MODE_POS:
    {
        APID_Set_Target(pid_pos, motor->tar_pos);
        APID_Set_Present(pid_pos, motor->cur_pos);
        APID_Hander(pid_pos, cycle);
        motor->tar_speed = APID_Get_Out(pid_pos);
    }
    case MOTOR_MODE_SPEED:
    {
        motor->acc_out = motor->tar_speed;
        break;
    }
    case MOTOR_MODE_TORQUE:
    {
        motor->acc_out = motor->tar_torque;
        break;
    }

    default:
    {
        break;
    }
    }
    return 0;
}
// 底层支持torque,speed,pos
int motor_behiver_4(int id, uint16_t mode, void *data, void *user_data)
{
    motor_t *motor = motor_get(id);
    apid_t *pid_torque = motor->pid_torque;
    apid_t *pid_speed = motor->pid_speed;
    apid_t *pid_pos = motor->pid_pos;
    PID_TYPE cycle = *((PID_TYPE *)(data));
    PID_TYPE tmpout = 0;
    switch (mode)
    {

    case MOTOR_MODE_POS:
    {
        motor->acc_out = motor->tar_pos;
        break;
    }
    case MOTOR_MODE_SPEED:
    {
        motor->acc_out = motor->tar_speed;
        break;
    }
    case MOTOR_MODE_TORQUE:
    {
        motor->acc_out = motor->tar_torque;
        break;
    }

    default:
    {
        break;
    }
    }
    return 0;
}

#define MOTOD_IS_POS_TIME(motor) (motor->time % motor->pos_tick == 0)
#define MOTOD_IS_SPEED_TIME(motor) (motor->time % motor->speed_tick == 0)
#define MOTOD_IS_TORQUE_TIME(motor) (motor->time % motor->torque_tick == 0)

static int __motor_read_feedback(motor_t *motor, PID_TYPE cycle)
{

    // 读取力矩
    if (MOTOD_IS_TORQUE_TIME(motor))
    {
        motor->ops->control(motor->id, MOTOR_MODE_TORQUE, &motor->cur_torque);
    }
    // 读取速度
    if (MOTOD_IS_SPEED_TIME(motor))
    {
        motor->ops->control(motor->id, MOTOR_MODE_SPEED, &motor->cur_speed);
    }
    // 读取角度
    if (MOTOD_IS_POS_TIME(motor))
    {
        motor->ops->control(motor->id, MOTOR_MODE_POS, &motor->cur_pos);
    }
    return 0;
}
int motor_read_feedback(int id, int cycle)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    __motor_read_feedback(motor, cycle);
    return 0;
}
int motor_handle(int id, float cycle)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    MOTOR_ASSERT(motor->ops);
    motor->time += cycle;
    if (motor->flag_passive_feedback)
    {
    }
    else
    {
        __motor_read_feedback(motor, cycle); // 更新反馈值
    }
		motor->flag_is_stop=0;
    if (motor->flag_is_stop)
    {
        motor->acc_out = 0;
    }
    else
    {
        motor->behaver(id, motor->flag_run_mode, &cycle, motor->ops->user_data); // 进行计算
    }
    motor->ops->driver(id, motor->flag_out_mode, (float *)&motor->acc_out, (motor->ops->user_data)); // 加载电机

    return 0;
}

int motor_feedback_torque(int id, float value)
{
    MOTOR_ASSERT_ID(id);
    motor_list[id].cur_torque = value;
    return 0;
}
int motor_feedback_speed(int id, float value)
{
    MOTOR_ASSERT_ID(id);
    motor_list[id].cur_speed = value;
    return 0;
}

int motor_feedback_pos(int id, float value)
{
    MOTOR_ASSERT_ID(id);
    motor_list[id].cur_pos = value;
    return 0;
}

/**
 * @brief 设置电机的速度
 *
 * @param id 电机id
 * @param value 速度值，单位r/min
 * @return int 获取状态
 */
int motor_start(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    motor->flag_is_stop = 0;
    return 0;
}

/**
 * @brief 设置电机的速度
 *
 * @param id 电机id
 * @param value 速度值，单位r/min
 * @return int 获取状态
 */
int motor_stop(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    motor->flag_is_stop = 1;
    return 0;
}

/**
 * @brief 设置电机的速度
 *
 * @param id 电机id
 * @param value 速度值，单位r/min
 * @return int 获取状态
 */
int motor_set_speed(int id, float value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);

    motor->flag_run_mode = MOTOR_MODE_SPEED;
    motor->tar_speed = value;
    //LOG_D("motor id:%d speed:%f", id, value);
    return M_EOK;

}
 /**
   * @brief 设置电机的位置
   *
   * @param id 电机id
   * @param value 位置值，单位rad
   * @return int 获取状态
   */
int motor_set_pos(int id, float value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);

    motor->flag_run_mode = MOTOR_MODE_POS;
    motor->tar_pos = value;

    return M_EOK;
}
/**
 * @brief 设置电机的力矩
 *
 * @param id    电机id
 * @param value     力矩值，单位N*m
 * @return int      获取状态
 */
int motor_set_torque(int id, float value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);

    motor->flag_run_mode = MOTOR_MODE_TORQUE;
    motor->tar_torque = value;

    return M_EOK;
}

/**
 * @brief   获取电机的速度
 *
 * @param id    电机id
 * @param value     速度值，单位r/min
 * @return int    获取状态
 */
float motor_get_speed(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->cur_speed;
}

/**
 * @brief   获取电机的速度
 *
 * @param id    电机id
 * @param value     速度值，单位r/min
 * @return int    获取状态
 */
apid_t *motor_get_pid_torque(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->pid_torque;
}
apid_t *motor_get_pid_speed(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->pid_speed;
}
apid_t *motor_get_pid_pos(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->pid_pos;
}
/**
 * @brief   获取电机的位置
 *
 * @param id    电机id
 * @param value     位置值，单位rad
 * @return int  获取状态
 */
float motor_get_pos(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->cur_pos;
}
/**
 * @brief   获取电机的力矩
 *
 * @param id    电机id
 * @param value     力矩值，单位N*m
 * @return int  获取状态
 */
float motor_get_torque(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    return motor->cur_torque;
}
void motor_shakdown(int id)
{
    static int time = 0;
    time++;
    motor_t *motor = motor_get(id);

    if (motor->flag_run_mode == MOTOR_MODE_TORQUE)
    {
        //  将力矩环路pid的当前值，设定值，输出值
        if (MOTOD_IS_TORQUE_TIME(motor))
            LOG_RAW("q:%f,%f,%f\r\n",
                    motor->pid_torque->parameter.target,
                    motor->pid_torque->parameter.present,
                    motor->pid_torque->parameter.out);
    }
    if (motor->flag_run_mode == MOTOR_MODE_SPEED)
    {
        //  将速度环路pid的当前值，设定值，输出值
        if (time % 10 == 0)
        {
            LOG_RAW("s:%f,%f,%f\r\n",
                    motor->pid_speed->parameter.target,
                    motor->pid_speed->parameter.present,
                    motor->pid_speed->parameter.out);
        }
    }
    if (motor->flag_run_mode == MOTOR_MODE_POS)
    {
        //  将位置环路pid的当前值，设定值，输出值
        if (time % 10 == 0)
        {
            LOG_RAW("p:%f,%f,%f\r\n",
                    motor->pid_pos->parameter.target,
                    motor->pid_pos->parameter.present,
                    motor->pid_pos->parameter.out);
        }
    }
}
int motor_updata_cfg(int id, int level)
{
    motor_t *motor = motor_get(id);

    switch (level)
    {
    case 0:
        motor->behaver = motor_behiver_1;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_NONE;
        motor->pid_torque = malloc(sizeof(apid_t));
        motor->pid_speed = malloc(sizeof(apid_t));
        motor->pid_pos = malloc(sizeof(apid_t));

        RT_ASSERT(motor->pid_torque);
        RT_ASSERT(motor->pid_speed);
        RT_ASSERT(motor->pid_pos);

        APID_Init(motor->pid_torque, PID_POSITION, motor->pid_torque->parameter.kp, motor->pid_torque->parameter.ki, motor->pid_torque->parameter.kd);
        APID_Init(motor->pid_speed, PID_POSITION, motor->pid_speed->parameter.kp, motor->pid_speed->parameter.ki, motor->pid_speed->parameter.kd);
        APID_Init(motor->pid_pos, PID_POSITION, motor->pid_pos->parameter.kp, motor->pid_pos->parameter.ki, motor->pid_pos->parameter.kd);

        break;
    case 1:
        motor->behaver = motor_behiver_2;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_TORQUE;

        RT_ASSERT(motor->pid_speed);
        RT_ASSERT(motor->pid_pos);

        APID_Init(motor->pid_speed, PID_POSITION, motor->pid_speed->parameter.kp, motor->pid_speed->parameter.ki, motor->pid_speed->parameter.kd);
        APID_Init(motor->pid_pos, PID_POSITION, motor->pid_pos->parameter.kp, motor->pid_pos->parameter.ki, motor->pid_pos->parameter.kd);
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        var_register(&(motor->tar_speed), "tarspeed", _f);
        var_register(&(motor->pid_speed->parameter.kp), "kp", _f);
        var_register(&(motor->pid_speed->parameter.ki), "ki", _f);
        var_register(&(motor->pid_speed->parameter.kd), "kd", _f);
        var_register(&(motor->tar_pos), "tarpos", _f);
        var_register(&(motor->pid_pos->parameter.kp), "kp1", _f);
        var_register(&(motor->pid_pos->parameter.ki), "ki1", _f);
        var_register(&(motor->pid_pos->parameter.kd), "kd1", _f);
        break;
    case 2:
        motor->behaver = motor_behiver_3;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_SPEED;
        motor->pid_pos = malloc(sizeof(apid_t));
        RT_ASSERT(motor->pid_pos);
        APID_Init(motor->pid_pos, PID_POSITION, motor->pid_pos->parameter.kp, motor->pid_pos->parameter.ki, motor->pid_pos->parameter.kd);
        break;
    case 3:
        motor->behaver = motor_behiver_4;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_POS;
        break;
    }
    MOTOR_ASSERT(motor);
    return 0;
}
void motor_set_passive_feedback(int id, bool is_true)
{
    motor_t *motor = motor_get(id);
    motor->flag_passive_feedback = is_true;
}
void motor_init(void)
{
    for (int i = 0; i < MOTOR_NUM; ++i)
    {
        motor_list[i].id = i;
        motor_list[i].time = 0;

        // TODO: 为什么要设置为1
        motor_list[i].pos_tick = 10;
        motor_list[i].speed_tick = 5;
        motor_list[i].torque_tick = 1;

        motor_list[i].flag_run_mode = MOTOR_MODE_IDEL;
        motor_list[i].flag_out_mode = MOTOR_MODE_IDEL;
        motor_updata_cfg(i, motor_list[i].flag_accept_level);
    }
}
enum
{
    MOTOR_OPT_ID = 0,
    MOTOR_OPT_TORQUE,
    MOTOR_OPT_SPEED,
    MOTOR_OPT_POS,
};
#ifndef FINSH_USING_OPTION_COMPLETION
static int cmd_motor(int argc, char **argv)

#else

void list_all_motor(void)
{
    for (int i = 0; i < MOTOR_NUM; ++i)
    {
        LOG_D("motor id:%d name:%s", i, motor_list[i].name);
    }
}
void set_get_torque(int argc, char **argv)
{
    if (argc >= 3)
    {
        int id = atoi(argv[2]);
        if (id >= MOTOR_NUM)
        {
            LOG_D("motor id error");
            return;
        }
        if (argc >= 4)
        {
            float value = atof(argv[3]);
            motor_set_torque(id, value);
        }
        else
        {
            LOG_D("motor id:%d torque:%f", id, motor_get_torque(id));
        }
    }
    else
    {
        LOG_D("motor id error");
    }
}
void set_get_speed(int argc, char **argv)
{
    if (argc >= 3)
    {
        int id = atoi(argv[2]);
        if (id >= MOTOR_NUM)
        {
            LOG_D("motor id error");
            return;
        }
        if (argc >= 4)
        {
            float value = atof(argv[3]);
            motor_set_speed(id, value);
        }
        else
        {
            LOG_D("motor id:%d speed:%f", id, motor_get_speed(id));
        }
    }
    else
    {
        LOG_D("motor id error");
    }
}
void set_get_pos(int argc, char **argv)
{
    if (argc >= 3)
    {
        int id = atoi(argv[2]);
        if (id >= MOTOR_NUM)
        {
            LOG_D("motor id error");
            return;
        }
        if (argc >= 4)
        {
            float value = atof(argv[3]);
            motor_set_pos(id, value);
        }
        else
        {
            LOG_D("motor id:%d pos:%f", id, motor_get_pos(id));
        }
    }
    else
    {
        LOG_D("motor id error");
    }
}

// static int cmd_motor(int argc, char **argv);
CMD_OPTIONS_STATEMENT(cmd_motor)
static int cmd_motor(int argc, char **argv)
{
    if (argc >= 2)
    {
        switch (MSH_OPT_ID_GET(cmd_motor))
        {
        case MOTOR_OPT_ID:
            list_all_motor();
            break;
        case MOTOR_OPT_TORQUE:
            set_get_torque(argc, argv);
            break;
        case MOTOR_OPT_SPEED:
            set_get_speed(argc, argv);
            break;
        case MOTOR_OPT_POS:
            set_get_pos(argc, argv);
            break;
        default:
            goto _usage;
        };
        return 0;
    }

_usage:
    rt_kprintf("Usage: m [options]\n");
    rt_kprintf("[options]:\n");
    MSH_OPT_DUMP(cmd_motor);
    return 0;
}
static int start_shakdown(int argc, char **argv)
{
    if (argc >= 2)
    {
        int id = atoi(argv[1]);
    }
    return 0;
}
MSH_CMD_EXPORT(start_shakdown, start motor shakdowm)

CMD_OPTIONS_NODE_START(cmd_motor)
CMD_OPTIONS_NODE(MOTOR_OPT_ID, id, list all motor id)
CMD_OPTIONS_NODE(MOTOR_OPT_TORQUE, torque, set / get torque)
CMD_OPTIONS_NODE(MOTOR_OPT_SPEED, speed, set / get speed)
CMD_OPTIONS_NODE(MOTOR_OPT_POS, pos, set / get pos)
CMD_OPTIONS_NODE_END
#endif /* FINSH_USING_OPTION_COMPLETION */
    MSH_CMD_EXPORT_ALIAS(cmd_motor, m, motor operate cmd, optenable);
