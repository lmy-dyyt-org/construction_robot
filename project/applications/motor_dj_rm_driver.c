/**
 * @file motor_dj_rm_driver.c
 * @author Dyyt587 (805207319@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-03
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 * 程序清单：这是一个 CAN 设备使用例程
 * 例程导出了 can_test 命令到控制终端
 * 命令调用格式：can_test can1
 * 命令解释：命令第二个参数是要使用的 CAN 设备名称，为空则使用默认的 CAN 设备
 * 程序功能：通过 CAN 设备发送一帧，并创建一个线程接收数据然后打印输出。
 */

#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */
#include "apid.h"
#include "motor_dj_rm_driver.h"

#define CAN_DEV_NAME "can1" /* CAN 设备名称 */

static struct rt_semaphore rx_sem; /* 用于接收消息的信号量 */
static rt_device_t can_dev;        /* CAN 设备句柄 */

#define DJ_MOTOR_NUMBER 11
// can1的 8个电机
static motor_measure_t motor_can1[DJ_MOTOR_NUMBER] = {0}; // 8个电机
// static PID_T motor_can1_speed_pid[DJ_MOTOR_NUMBER] = {0}; //
// static PID_T motor_can1_pos_pid[DJ_MOTOR_NUMBER] = {0}; //
// can2的 8个电机
static motor_measure_t motor_can2[DJ_MOTOR_NUMBER] = {0};

#ifdef MOTOR_DJ_M3508_ID1_CAN1
#define MOTOR_DJ_M3508_ID1_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[0]                 \
}                                   
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN1
#define MOTOR_DJ_M3508_ID2_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[1]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN1
#define MOTOR_DJ_M3508_ID3_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[2]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN1

#define MOTOR_DJ_M3508_ID4_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[3]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN1
#define MOTOR_DJ_M3508_ID5_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[4]\
}
#endif

#ifdef MOTOR_DJ_M3508_ID6_CAN1
#define MOTOR_DJ_M3508_ID6_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[5]\
}

#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN1
#define MOTOR_DJ_M3508_ID7_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[6]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN1
#define MOTOR_DJ_M3508_ID8_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can1[7]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID1_CAN2
#define MOTOR_DJ_M3508_ID1_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[0]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN2
#define MOTOR_DJ_M3508_ID2_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[1]                 \
}
#endif

#ifdef MOTOR_DJ_M3508_ID3_CAN2
#define MOTOR_DJ_M3508_ID3_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[2]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN2
#define MOTOR_DJ_M3508_ID4_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[3]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN2
#define MOTOR_DJ_M3508_ID5_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[4]                 \        
}
#endif
#ifdef MOTOR_DJ_M3508_ID6_CAN2
#define MOTOR_DJ_M3508_ID6_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[5]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN2
#define MOTOR_DJ_M3508_ID7_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[6]                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN2
#define MOTOR_DJ_M3508_ID8_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &motor_can2[7]                 \
}
#endif


//can1 的
int16_t iq1_low[4], iq1_high[4],iq1_uhigh[4];

int16_t iq2_low[4], iq2_high[4], iq2_uhigh[4];

int motor_dj_driver(int id, MOTOR_VALUE_TYPE mode, void *value, void *user_data)
{
    motor_t *motor = motor_get(id);
    struct rt_can_msg msg = {0};
    motor_measure_t *__motor = (motor_measure_t *)motor->ops->user_data;

    switch (mode)
    {
    case MOTOR_MODE_TORQUE:
        /*设置力矩/电流值*/
        // tmpout = apid_calc(pid_torque, *((PID_TYPE *)(value)), motor->real_current);
        /* 发送一帧 CAN 数据 */
        msg.ide = RT_CAN_STDID; /* 标准格式 */
        msg.rtr = RT_CAN_DTR;   /* 数据帧 */
        msg.len = 8;            /* 数据长度为 8 */
        msg.id = __motor->id;
		if((msg.id - CAN_Motor1_ID)>4){
			        uint16_t tt = (__motor->id - CAN_Motor1_ID) % 4;
        iq1_high[tt] = *((int16_t *)(value));
        /* 待发送的 8 字节数据 */
        msg.data[0] = iq1_high[0] >> 8;
        msg.data[1] = iq1_high[0];
        msg.data[2] = iq1_high[1] >> 8;
        msg.data[3] = iq1_high[1];
        msg.data[4] = iq1_high[2] >> 8;
        msg.data[5] = iq1_high[2];
        msg.data[6] = iq1_high[3] >> 8;
        msg.data[7] = iq1_high[3];
		}else{
			        uint16_t tt = (__motor->id - CAN_Motor1_ID) % 4;
        iq1_low[tt] = *((int16_t *)(value));
        /* 待发送的 8 字节数据 */
        msg.data[0] = iq1_low[0] >> 8;
        msg.data[1] = iq1_low[0];
        msg.data[2] = iq1_low[1] >> 8;
        msg.data[3] = iq1_low[1];
        msg.data[4] = iq1_low[2] >> 8;
        msg.data[5] = iq1_low[2];
        msg.data[6] = iq1_low[3] >> 8;
        msg.data[7] = iq1_low[3];
		}

        rt_size_t size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
        if (size == 0)
        {
            rt_kprintf("can dev write data failed!\n");
        }
        break;
    case MOTOR_MODE_SPEED:

        break;
    case MOTOR_MODE_POS:

        break;
    default:
        break;
    }
    return 0;
}
int motor_dj_ctr(int id, MOTOR_VALUE_TYPE mode, float *data)
{
    motor_t *motor = motor_get(id);
    motor_measure_t *__motor = (motor_measure_t *)motor->ops->user_data;

    switch (mode)
    {
    case MOTOR_MODE_TORQUE:
        /*返回力矩/电流值*/
        *data = __motor->real_current;
        break;
    case MOTOR_MODE_SPEED:
        // /*返回速度值r/min*/
        *data = __motor->speed_rpm;
        break;
    case MOTOR_MODE_POS:
        // /*返回位置 rad*/
        *data = __motor->total_angle;
        break;
    default:
        break;
    }
    return 0;
}

/* 接收数据回调函数 */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}
/**
    Rx_Data[0] 转子机械角度高8位
    Rx_Data[1] 转子机械角度低8位
    Rx_Data[2] 转子转速高8位
    Rx_Data[3] 转子转速低8位
    Rx_Data[4] 实际转矩电流高8位
    Rx_Data[5] 实际转矩电流低8位
    Rx_Data[6] 电机温度
    Rx_Data[7] 空Null

    发送频率 1KHz
    转子机械角度 0 ~ 8191 (对应转子角度0~360)
    转子转速单位 RPM
    电机温度单位 °C

last_angle  上次角度更新
angle       转子机械角度高8位和第八位
speed_rpm   转子转速高8位和低八位
real_current实际输出转矩高8位和低8位
temperature 温度
total_angle 总角度
 */
void dj_motor_measure_updata(motor_measure_t *motor, uint8_t Rx_Data[])
{
    motor->last_angle = motor->angle;                           // 上次角度更新
    motor->angle = (uint16_t)(Rx_Data[0] << 8 | Rx_Data[1]);    // 转子机械角度高8位和第八位
    motor->speed_rpm = (int16_t)(Rx_Data[2] << 8 | Rx_Data[3]); // 转子转速高8位和低八位

    motor->real_current = (int16_t)(Rx_Data[4] << 8 | Rx_Data[5]); // 实际输出转矩高8位和低8位
    motor->temperature = Rx_Data[6];                               // 温度     //Null

    if (motor->angle - motor->last_angle > 4096)
        motor->round_cnt--;
    else if (motor->angle - motor->last_angle < -4096)
        motor->round_cnt++;
    motor->total_angle = motor->round_cnt * 8191 + motor->angle - motor->offset_angle;
}

void get_motor_offset(motor_measure_t *motor, uint8_t Rx_Data[])
{
    motor->offset_angle = motor->angle = (uint16_t)(Rx_Data[0] << 8 | Rx_Data[1]);
}

void dj_motor_BackToZero(motor_measure_t *motor)
{
    motor->offset_angle = motor->angle;
    motor->round_cnt = 0;
}

static void can_rx_thread(void *parameter)
{
    int i;
    rt_err_t res;
    struct rt_can_msg rxmsg = {0};

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(can_dev, can_rx_call);

    // #ifdef RT_CAN_USING_HDR
    //     struct rt_can_filter_item items[5] =
    //     {
    //         RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
    //         RT_CAN_FILTER_ITEM_INIT(0x300, 0, 0, 0, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x300~0x3ff，hdr 为 - 1 */
    //         RT_CAN_FILTER_ITEM_INIT(0x211, 0, 0, 0, 0x7ff, RT_NULL, RT_NULL), /* std,match ID:0x211，hdr 为 - 1 */
    //         RT_CAN_FILTER_STD_INIT(0x486, RT_NULL, RT_NULL),                  /* std,match ID:0x486，hdr 为 - 1 */
    //         {0x555, 0, 0, 0, 0x7ff, 7,}                                       /* std,match ID:0x555，hdr 为 7，指定设置 7 号过滤表 */
    //     };
    //     struct rt_can_filter_config cfg = {5, 1, items}; /* 一共有 5 个过滤表 */
    //     /* 设置硬件过滤表 */
    //     res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    //     RT_ASSERT(res == RT_EOK);
    // #endif

    while (1)
    {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rxmsg.hdr_index = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
        /* 打印数据 ID 及内容 */
        rt_kprintf("ID:%x ", rxmsg.id);
        if (rxmsg.id < 0x209)
            i = rxmsg.id - CAN_Motor1_ID;
        else
            i = rxmsg.id - CAN_6020_ID1 + 4;
        if (motor_can1[i].msg_cnt <= 50) // 上电后接收50次矫正 50次之后正常接收数据
        {
            motor_can1[i].msg_cnt++;
            get_motor_offset(&motor_can1[i], rxmsg.data);
        }
        else
            dj_motor_measure_updata(&motor_can1[i], rxmsg.data);

        // for (i = 0; i < 8; i++)
        // {
        //     rt_kprintf("%2x ", rxmsg.data[i]);
        // }

        // rt_kprintf("\n");
    }
}

#define CAN_DEV_NAME "can1" /* CAN 设备名称 */
int motor_tt_init(void)
{
    struct rt_can_msg msg = {0};
    rt_err_t res;
    rt_size_t size;
    rt_thread_t thread;


    for(int i = 0; i < DJ_MOTOR_NUMBER; i++)
    {
        motor_can1[i].id = CAN_Motor1_ID + i;
        motor_can1[i].msg_cnt = 0;
        motor_can1[i].angle = 0;
        motor_can1[i].last_angle = 0;
        motor_can1[i].offset_angle = 0;
        motor_can1[i].round_cnt = 0;
        motor_can1[i].speed_rpm = 0;
        motor_can1[i].real_current = 0;
        motor_can1[i].temperature = 0;
        motor_can1[i].total_angle = 0;
    }
    for(int i = 0; i < DJ_MOTOR_NUMBER; i++)
    {
        motor_can2[i].id = CAN_Motor5_ID + i;
        motor_can2[i].msg_cnt = 0;
        motor_can2[i].angle = 0;
        motor_can2[i].last_angle = 0;
        motor_can2[i].offset_angle = 0;
        motor_can2[i].round_cnt = 0;
        motor_can2[i].speed_rpm = 0;
        motor_can2[i].real_current = 0;
        motor_can2[i].temperature = 0;
        motor_can2[i].total_angle = 0;
    }
    for(int i = 0; i < 8; i++)
    {
        motor_can1[i];
    }
    /* 查找 CAN 设备 */
    can_dev = rt_device_find(CAN_DEV_NAME);
    if (!can_dev)
    {
        rt_kprintf("find %s failed!\n", CAN_DEV_NAME);
        return -RT_ERROR;
    }

    /* 初始化 CAN 接收信号量 */
    rt_sem_init(&rx_sem, "can_m_dj_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及中断发送方式打开 CAN 设备 */
    res = rt_device_open(can_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);
    /* 设置 CAN 通信的波特率为 500kbit/s*/
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_BAUD, (void *)CAN1MBaud);
    RT_ASSERT(res == RT_EOK);

    /* 创建数据接收线程 */
    thread = rt_thread_create("m_dj_driver", can_rx_thread, RT_NULL, 1024, 25, 10);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }
    return 0;
}
INIT_COMPONENT_EXPORT(motor_tt_init);



