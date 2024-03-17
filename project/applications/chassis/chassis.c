/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 09:10:28
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-16 21:35:39
 * @FilePath: \project\applications\chassis\chassis.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis.h"
#include "stdint.h"
#include "stddef.h"
#include "ulog.h"


int chassis_init(chassis_t *chassis, chassis_ops_t *ops)
{
    chassis->ops = ops;
    chassis->run_status = CHASSIS_IDLE;
		return 0;

}

int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data)
{
    /* 将数据更新到target 改变status */
    chassis->target.pos = *data;
    chassis->run_status = CHASSIS_POS;
		return 0;

}

int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data)
{
    /* 将数据更新到target 改变status */
    chassis->target.speed = *data;
    chassis->run_status = CHASSIS_SPEED;
	return 0;
}

const chassis_pos_t *chassis_get_pos(chassis_t *chassis)
{
    /* 返回当前位置 */
    return &chassis->present.pos;
}

const chassis_speed_t *chassis_get_speed(chassis_t *chassis)
{
    /* 返回当前速度 */
    return &chassis->present.speed;
}

/**
 * @brief 底盘抽象层的处理函数
 *
 * @param chassis 底盘对象
 * @param time_ms 全局时间，单位ms
 * @return int
 */
int chassis_handle(chassis_t *chassis, int time_ms)
{
    if(chassis == NULL)
    {
        CHASSIS_LOG("chassis is NULL\n");
        return -1;
    }
    
    /* 更新当前数据 */
    const void *output = chassis->ops->date;
    chassis->ops->driver(NULL, output);
    chassis->ops->module(chassis, NULL, output);

    /* 设置数据 */
    const void *input = chassis->ops->date;
    chassis->ops->module(chassis, output, NULL);
    chassis->ops->driver(output, NULL);

    /* 根据状态执行相应的操作 */
    switch (chassis->run_status)
    {
    case CHASSIS_IDLE:
        break;
    case CHASSIS_SPEED:
        break;
    case CHASSIS_POS:
        break;
    default:
        CHASSIS_LOG("chassis status error(%d)\n", chassis->run_status);
        break;
    }
		return 0;
}