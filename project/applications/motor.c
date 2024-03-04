/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-03 15:24:57
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-04 22:03:31
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
static int motor_id = 0; // 用于分配电机id

#define MOTOR_ASSERT(x) \
    if (!(x))           \
        while (1)       \
            ;
                         // 电机操作函数的断言
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
//     for (int i = 0; i < 10; i++)
//     {
//         if (motor_list[i].id == id)
//         {
//             return &motor_list[i];
// }
//}
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

// /**
//  * @brief 创建一个电机
//  *
//  * @param ops 电机操作函数
//  * @return int 返回实际分配的id
//  */
// int motor_create(motor_ops_t *ops)
// {
//     MOTOR_ASSERT(ops);
//     // 创建电机
//     motor_t *motor = (motor_t *)MOTOR_MALLOC(sizeof(motor_t));
//     if (motor)
//     {
//         motor_init(motor, ops);
//         return motor->id;
//     }
//     else
//     {
//         return -M_ENOMEM;
//     }
// }
#define MOTOD_IS_POS_TIME(motor) (motor->time % motor->pos_tick == 0)
#define MOTOD_IS_SPEED_TIME(motor) (motor->time % motor->speed_tick == 0)
#define MOTOD_IS_TORQUE_TIME(motor) (motor->time % motor->torque_tick == 0)

static int motor_read_feedback(motor_t *motor, PID_TYPE cycle)
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

int motor_handle(int id, float cycle)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    MOTOR_ASSERT(motor->ops);
    motor->time += cycle;
    if(motor->flag_passive_feedback){}else{
        motor_read_feedback(motor, cycle);                                                               // 更新反馈值
    }
    motor->behaver(id, motor->flag_run_mode, &cycle, motor->ops->user_data);                         // 进行计算
    motor->ops->driver(id, motor->flag_out_mode, (float *)&motor->acc_out, (motor->ops->user_data)); // 加载电机
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

    return M_EOK;

} /**
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
int motor_get_speed(int id, float *value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    *value = motor->cur_speed;
    return M_EOK;
}
/**
 * @brief   获取电机的位置
 *
 * @param id    电机id
 * @param value     位置值，单位rad
 * @return int  获取状态
 */
int motor_get_pos(int id, float *value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    *value = motor->cur_pos;
    return M_EOK;
}
/**
 * @brief   获取电机的力矩
 *
 * @param id    电机id
 * @param value     力矩值，单位N*m
 * @return int  获取状态
 */
int motor_get_torque(int id, float *value)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    *value = motor->cur_torque;
    return M_EOK;
}

int motor_updata_cfg(int id, int level)
{
    motor_t *motor = motor_get(id);

    switch (level)
    {
    case 0:
        motor->behaver = motor_behiver_1;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_NONE;
        break;
    case 1:
        motor->behaver = motor_behiver_2;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_TORQUE;
        break;
    case 2:
        motor->behaver = motor_behiver_3;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_SPEED;
        break;
    case 3:
        motor->behaver = motor_behiver_4;
        motor->flag_out_mode = MOTOR_CONTROL_SUPPORT_POS;
        break;
    }
    MOTOR_ASSERT(motor);
    return 0;
}

void motor_init(void)
{
    for(int i=0;i<MOTOR_NUM;++i)
    {
        motor_list[i].id = i;
        motor_list[i].time = 0;

        //TODO: 为什么要设置为1
        motor_list[i].pos_tick = 1;
        motor_list[i].speed_tick = 1;
        motor_list[i].torque_tick = 1;

        motor_list[i].flag_run_mode = MOTOR_MODE_IDEL;
        motor_list[i].flag_out_mode = MOTOR_MODE_IDEL;
        motor_updata_cfg(i,motor_list[i].flag_accept_level);
    }
}