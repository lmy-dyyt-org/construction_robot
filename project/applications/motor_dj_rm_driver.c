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
#include "ulog.h"
#include "motor_cfg.h"
#define CAN_DEV_NAME "can1" /* CAN 设备名称 */

static struct rt_semaphore rx_sem; /* 用于接收消息的信号量 */
static rt_device_t can_dev;        /* CAN 设备句柄 */

#define DJ_MOTOR_MOTOR_ID(index, __id, __can_id) [index] = {             \
                                                     .id = __id,         \
                                                     .can_id = __can_id, \
}

motor_measure_t dj_motors[DJ_M_NUM] = {
#ifdef MOTOR_DJ_M3508_ID1_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_1, M3508_1_CAN1, CAN_Motor1_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_2, M3508_2_CAN1, CAN_Motor2_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_3, M3508_3_CAN1, CAN_Motor3_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_4, M3508_4_CAN1, CAN_Motor4_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_5, M3508_5_CAN1, CAN_Motor5_ID),
#endif

#ifdef MOTOR_DJ_M3508_ID6_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_6, M3508_6_CAN1, CAN_Motor6_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_7, M3508_7_CAN1, CAN_Motor7_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_8, M3508_8_CAN1, CAN_Motor8_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID1_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_1, M3508_1_CAN2, CAN_Motor1_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_2, M3508_2_CAN2, CAN_Motor2_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_3, M3508_3_CAN2, CAN_Motor3_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_4, M3508_4_CAN2, CAN_Motor4_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN2

    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_5, M3508_5_CAN2, CAN_Motor5_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID6_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_6, M3508_6_CAN2, CAN_Motor6_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_7, M3508_7_CAN2, CAN_Motor7_ID),
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_8, M3508_8_CAN2, CAN_Motor8_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID1_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_1, M2006_1_CAN1, CAN_Motor1_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID2_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_2, M2006_2_CAN1, CAN_Motor2_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID3_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_3, M2006_3_CAN1, CAN_Motor3_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID4_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_4, M2006_4_CAN1, CAN_Motor4_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID5_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_5, M2006_5_CAN1, CAN_Motor5_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID6_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_6, M2006_6_CAN1, CAN_Motor6_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID7_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_7, M2006_7_CAN1, CAN_Motor7_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID8_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_8, M2006_8_CAN1, CAN_Motor8_ID),
#endif

#ifdef MOTOR_DJ_M2006_ID1_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_1, M2006_1_CAN2, CAN_Motor1_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID2_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_2, M2006_2_CAN2, CAN_Motor2_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID3_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_3, M2006_3_CAN2, CAN_Motor3_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID4_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_4, M2006_4_CAN2, CAN_Motor4_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID5_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_5, M2006_5_CAN2, CAN_Motor5_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID6_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_6, M2006_6_CAN2, CAN_Motor6_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID7_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_7, M2006_7_CAN2, CAN_Motor7_ID),
#endif
#ifdef MOTOR_DJ_M2006_ID8_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_8, M2006_8_CAN2, CAN_Motor8_ID),
#endif

#ifdef MOTOR_DJ_M6020_ID1_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_5, M6020_1_CAN1, CAN_6020_ID1),
#endif
#ifdef MOTOR_DJ_M6020_ID2_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_6, M6020_2_CAN1, CAN_6020_ID2),
#endif
#ifdef MOTOR_DJ_M6020_ID3_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_7, M6020_3_CAN1, CAN_6020_ID3),
#endif
#ifdef MOTOR_DJ_M6020_ID4_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_8, M6020_4_CAN1, CAN_6020_ID4),
#endif
#ifdef MOTOR_DJ_M6020_ID5_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_9, M6020_5_CAN1, CAN_6020_ID5),
#endif
#ifdef MOTOR_DJ_M6020_ID6_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_10, M6020_6_CAN1, CAN_6020_ID6),
#endif
#ifdef MOTOR_DJ_M6020_ID7_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_11, M6020_7_CAN1, CAN_6020_ID7),
#endif
#ifdef MOTOR_DJ_M6020_ID8_CAN1
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN1_12, M6020_8_CAN1, CAN_6020_ID8),
#endif

#ifdef MOTOR_DJ_M6020_ID1_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_5, M6020_1_CAN2, CAN_6020_ID1),
#endif
#ifdef MOTOR_DJ_M6020_ID2_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_6, M6020_2_CAN2, CAN_6020_ID2),
#endif
#ifdef MOTOR_DJ_M6020_ID3_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_7, M6020_3_CAN2, CAN_6020_ID3),
#endif
#ifdef MOTOR_DJ_M6020_ID4_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_8, M6020_4_CAN2, CAN_6020_ID4),
#endif
#ifdef MOTOR_DJ_M6020_ID5_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_9, M6020_5_CAN2, CAN_6020_ID5),
#endif
#ifdef MOTOR_DJ_M6020_ID6_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_10, M6020_6_CAN2, CAN_6020_ID6),
#endif
#ifdef MOTOR_DJ_M6020_ID7_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_11, M6020_7_CAN2, CAN_6020_ID7),
#endif
#ifdef MOTOR_DJ_M6020_ID8_CAN2
    DJ_MOTOR_MOTOR_ID(DJ_M_CAN2_12, M6020_8_CAN2, CAN_6020_ID8),
#endif

};
// 写一个二分查找,通过can_id查找到电机对应的结构体
motor_measure_t *motor_get_by_canid(uint16_t can_id)
{
    uint8_t left = 0, right = DJ_M_NUM - 1;
    uint8_t mid;
    while (left <= right)
    {
        mid = (left + right) / 2;
        if (dj_motors[mid].can_id == can_id)
            return &dj_motors[mid];
        else if (dj_motors[mid].can_id < can_id)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return RT_NULL;
}

// can1 的
int16_t iq1_low[4], iq1_high[4], iq1_uhigh[4];

int16_t iq2_low[4], iq2_high[4], iq2_uhigh[4];

int motor_dj_driver(int id, uint16_t mode, float *value, void *user_data)
{
    motor_t *motor = motor_get(id);
    struct rt_can_msg msg = {0};
    motor_measure_t *__motor = (motor_measure_t *)motor->ops->user_data;
    int16_t tmpout = *value;
    switch (mode)
    {
    case MOTOR_MODE_TORQUE:
        /*设置力矩/电流值*/
        /* 发送一帧 CAN 数据 */
        msg.ide = RT_CAN_STDID; /* 标准格式 */
        msg.rtr = RT_CAN_DTR;   /* 数据帧 */
        msg.len = 8;            /* 数据长度为 8 */
        uint16_t tt = (__motor->can_id - CAN_Motor1_ID) % 4;
        // LOG_D("tt %d value %f",tt,value);

        if ((msg.id - CAN_Motor1_ID) > 8) // 云台电机
        {
        }
        else if ((msg.id - CAN_Motor1_ID) > 4)
        {
            iq1_high[tt] = tmpout;
            /* 待发送的 8 字节数据 */
            msg.id = CAN_Motor_ALL_ID2;
            msg.data[0] = iq1_high[0] >> 8;
            msg.data[1] = iq1_high[0];
            msg.data[2] = iq1_high[1] >> 8;
            msg.data[3] = iq1_high[1];
            msg.data[4] = iq1_high[2] >> 8;
            msg.data[5] = iq1_high[2];
            msg.data[6] = iq1_high[3] >> 8;
            msg.data[7] = iq1_high[3];
        }
        else
        {
            iq1_low[tt] = tmpout;
            /* 待发送的 8 字节数据 */
            msg.id = CAN_Motor_ALL_ID;
            msg.data[0] = iq1_low[0] >> 8;
            msg.data[1] = iq1_low[0];
            msg.data[2] = iq1_low[1] >> 8;
            msg.data[3] = iq1_low[1];
            msg.data[4] = iq1_low[2] >> 8;
            msg.data[5] = iq1_low[2];
            msg.data[6] = iq1_low[3] >> 8;
            msg.data[7] = iq1_low[3];
        }
        // msg.data[3]=12;
        rt_size_t size = rt_device_write(can_dev, 0, &msg, sizeof(msg));
        if (size == 0)
        {
            rt_kprintf("can dev write data failed!\n");
        }
        break;
    case MOTOR_MODE_SPEED:
    case MOTOR_MODE_POS:
    default:
        LOG_E("not support");
        break;
    }
    return 0;
}
/**
 * @brief 针对2006电机的控制
 * 
 * @param id 电机抽象层id单号
 * @param cmd 控制命令
 * @param arg 命令参数
 * @return int 
 */
int motor_dj_ctr(int id, uint16_t cmd, float *arg)
{
    motor_t *motor = motor_get(id);
    motor_measure_t *__motor = (motor_measure_t *)motor->ops->user_data;

    switch (cmd)
    {
    case MOTOR_MODE_TORQUE:
        /*返回力矩/电流值*/
        *arg = __motor->real_current;
        break;
    case MOTOR_MODE_SPEED:
        // /*返回速度值r/min*/
        *arg = __motor->speed_rpm;
        break;
    case MOTOR_MODE_POS:
        // /*返回位置 rad*/
        *arg = __motor->total_angle;
        break;
    case MOTOR_MODE_TEMP:
        // /*返回温度*/
        *arg = 42;
        break;
    default:
        break;
    }
    return 0;
}

/* 接收数据回调函数 */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size);

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
void dj_motor_BackToZero(motor_measure_t *motor)
{
    motor->offset_angle = motor->angle;
    motor->round_cnt = 0;
}
rt_err_t ind_dj_can_motor_callback(rt_device_t dev, void *args, rt_int32_t hdr, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

static void can_rx_thread(void *parameter)
{
    int i;
    rt_err_t res;
    struct rt_can_msg rxmsg = {0};

    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(can_dev, can_rx_call);

#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[] = {
        {.id = 0x200, .ide = 0, .rtr = 0, .mode = 0, .mask = 0x7f0, .hdr_bank = 0, .rxfifo = CAN_RX_FIFO0, .ind = ind_dj_can_motor_callback, .args = RT_NULL}};
    
    struct rt_can_filter_config cfg = {sizeof(items) / sizeof(struct rt_can_filter_item), 1, items}; /* 过滤表 */
    /* 设置硬件过滤表 */
    res = rt_device_control(can_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif

    while (1)
    {
        /* hdr 值为 0，从 过滤器读取数据 */
        rxmsg.hdr_index = 0;
        /* 阻塞等待接收信号量 */
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(can_dev, 0, &rxmsg, sizeof(rxmsg));
        /* 打印数据 ID 及内容 */
        // rt_kprintf("ID:%x ", rxmsg.id);

        motor_measure_t *motor_measure = motor_get_by_canid(rxmsg.id);

        if (motor_measure->msg_cnt <= 50) // 上电后接收50次矫正 50次之后正常接收数据
        {
            motor_measure->msg_cnt++;
            motor_measure->offset_angle = motor_measure->angle = (uint16_t)(rxmsg.data[0] << 8 | rxmsg.data[1]);
        }
        else
        {
            motor_measure->last_angle = motor_measure->angle;                         // 上次角度更新
            motor_measure->angle = (uint16_t)(rxmsg.data[0] << 8 | rxmsg.data[1]);    // 转子机械角度高8位和第八位
            motor_measure->speed_rpm = (int16_t)(rxmsg.data[2] << 8 | rxmsg.data[3]); // 转子转速高8位和低八位

            motor_measure->real_current = (int16_t)(rxmsg.data[4] << 8 | rxmsg.data[5]); // 实际输出转矩高8位和低8位
            motor_measure->temperature = rxmsg.data[6];                                  // 温度     //Null

            if (motor_measure->angle - motor_measure->last_angle > 4096)
                motor_measure->round_cnt--;
            else if (motor_measure->angle - motor_measure->last_angle < -4096)
                motor_measure->round_cnt++;
            motor_measure->total_angle = motor_measure->round_cnt * 8191 + motor_measure->angle - motor_measure->offset_angle;
        }

        LOG_D("id %d, speed %d total_angle %d", motor_measure->id, motor_measure->speed_rpm, motor_measure->total_angle);

        int id = motor_measure->id;

        // motor_feedback_torque(id, motor_measure->real_current);
        // motor_feedback_speed(id, motor_measure->speed_rpm);
        // motor_feedback_pos(id, motor_measure->total_angle);
        uint16_t current = (int16_t)(rxmsg.data[4] << 8 | rxmsg.data[5]);
        uint16_t speed_rpm = (int16_t)(rxmsg.data[2] << 8 | rxmsg.data[3]);
        uint16_t pos = 0;
        motor_feedback_torque(id, current);
        motor_feedback_speed(id, speed_rpm);
        motor_feedback_pos(id, motor_measure->total_angle);

        float speed_rpm1 = motor_measure->speed_rpm;
        float total_angle = motor_measure->total_angle;
        // 更新数据
        // motor_t *motor = motor_get(id);
        motor_set_torque(id, 100);
        motor_handle(id, 1);

        // motor_dj_ctr(0, MOTOR_MODE_TORQUE, &motor_measure->real_current);
        // motor_dj_ctr(0, MOTOR_MODE_SPEED, &speed_rpm);
        // motor_dj_ctr(0, MOTOR_MODE_POS, &total_angle);
        // float cur = 0;
        // motor_dj_driver(0, MOTOR_MODE_TORQUE, &cur, &motor_can1[i]);
    }
}
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{

    LOG_W("can_rx_call unknown id data");
    return RT_EOK;
}

int motor_tt_init(void)
{
    struct rt_can_msg msg = {0};
    rt_err_t res;
    rt_size_t size;
    rt_thread_t thread;

    motor_init();
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
    thread = rt_thread_create("m_dj_driver", can_rx_thread, RT_NULL, 4096, 25, 10);
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
