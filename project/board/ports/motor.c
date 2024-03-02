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
            ;                      // 电机操作函数的断言
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

static motor_t *motor_list[10]; // 电机列表

/**
 * @brief 获取电机
 *
 * @param id 电机id
 * @return motor_t* 返回电机指针
 */
static motor_t *motor_get(int id)
{
    for (int i = 0; i < 10; i++)
    {
        if (motor_list[i]->id == id)
        {
            return motor_list[i];
        }
    }
    return NULL;
}
/**
 * @brief 创建一个电机
 *
 * @param ops 电机操作函数
 * @return int 返回实际分配的id
 */
int motor_create(motor_ops_t *ops)
{
    MOTOR_ASSERT(ops);
    // 创建电机
    motor_t *motor = (motor_t *)MOTOR_MALLOC(sizeof(motor_t));
    if (motor)
    {
        motor_init(motor, ops);
        return motor->id;
    }
    else
    {
        return -M_ENOMEM;
    }
}
int motor_handle(int id, int cycle)
{
    return 0;
}
/**
 * @brief 初始化电机
 *
 * @param motor
 * @param ops
 * @return int
 */
int motor_init(motor_t *motor, motor_ops_t *ops)
{
    MOTOR_ASSERT(motor);
    MOTOR_ASSERT(ops);
    motor->ops = ops;
    motor->id = motor_id;
    motor_list[motor_id++] = motor;
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

    motor->tar_speed = value;
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

    motor->tar_pos = value;
    motor->flag_run_mode = MOTOR_MODE_POS;
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

    motor->tar_torque = value;
    motor->flag_run_mode = MOTOR_MODE_TORQUE;

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

/**
 * @brief   控制电机的配置
 *
 * @param id    电机id
 * @param mode 电机支持的控制方式
 * @param data 可能携带的数据
 * @return int 获取状态
 */
int motor_control(int id, MOTOR_VALUE_TYPE mode, void *data)
{
    motor_t *motor = motor_get(id);
    switch (mode)
    {
    case MOTOR_CONTROL_SUPPORT_TORQUE:
    }
}

/**
 * @brief   配置电机的配置
 *
 * @param id    电机id
 * @param mode 电机支持的控制方式
 * @param data 可能携带的数据
 * @return int 获取状态
 */
static int motor_support_torque(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    motor->flag_accept_torque =1;
    return 0;
}
static int motor_support_speed(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    motor->flag_accept_speed =1;
    return 0;
}
static int motor_support_pos(int id)
{
    motor_t *motor = motor_get(id);
    MOTOR_ASSERT(motor);
    motor->flag_accept_pos =1;
    return 0;
}