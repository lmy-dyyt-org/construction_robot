/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-11 00:05:09
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-17 22:45:49
 * @FilePath: \abus_v2.0.1\abus_v2.0.1\abus_topic.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "abus_topic.h"
#include "ulog.h"

int abus_topic_init(abus_topic_t *topic, abus_topic_init_t *init)
{
    topic->name = init->name;
    topic->datafifo = afifo_init(init->buf, init->buf_size, 0);
    INIT_LIST_HEAD(&topic->sync_accounter);
    INIT_LIST_HEAD(&topic->async_accounter);
    topic->msg_size = init->msg_size;
    
    return 0;
}
//int abus_acc_init(abus_accounter_t *acc, abus_acc_init_t *init)
//{

//    return 0;
//}
int abus_topic_subscribe(abus_topic_t *topic, abus_accounter_t *acc, abus_sub_flag flag)
{
    acc->flag = flag;
    acc->topic = topic;
    if (acc->flag.is_sync)
    {
        // sync
        list_add(&acc->head, &topic->sync_accounter);
    }
    else
    {
        LOG_D("abus_topic_subscribe  async\n");
        // async
        list_add(&acc->head, &topic->async_accounter);
    }
    return 0;
}

int abus_topic_unsubscribe(abus_topic_t *topic, abus_accounter_t *acc)
{
    acc->topic = 0;
    list_del(&acc->head);

    return 0;
}

int abus_public(abus_accounter_t *acc, void *msg)
{

    // 将数据放入topic
     uint32_t acc_in = afifo_in(acc->topic->datafifo,msg,acc->topic->msg_size);
     if (acc_in != acc->topic->msg_size)
     {
         //TODO:
         LOG_D("abus_public  data in error acc in %d will in %d\n",acc_in,acc->topic->msg_size);
         //while (1);
     }
    // 遍历sync链表
    alist_head_t* pos=0;
    list_for_each(pos, &acc->topic->sync_accounter)
    {
        abus_accounter_t* sync = list_entry(pos, abus_accounter_t, head);
        if (sync->flag.is_sync)
        {
            // sync
            if (sync->datafifo)
            {
                //将数据复制到里面
                uint32_t sync_in = afifo_in(sync->datafifo,msg,acc->topic->msg_size);
                if (sync_in != sync->topic->msg_size)
                {   
                    LOG_D("abus_public data in error sync in %d will in %d\n",sync_in,acc->topic->msg_size);
                    while(1);
                }
            }
            if (sync->callback)sync->callback(sync->topic);
            LOG_D("abus_public  sub \n");
        }
        afifo_add_out(acc->topic->datafifo,acc->topic->msg_size);
    }



    //    //sync
    //    if (sync->datafifo) {
    //        //将数据复制到里面

    //    }
    //    if (sync->callback)acc->callback(acc->topic);

    //}
    return 0;
}
