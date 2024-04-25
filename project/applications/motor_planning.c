/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-04-21 00:14:50
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-04-23 19:43:43
 * @FilePath: \project\applications\motor_planning.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "motor_planning.h"
#include <rtthread.h>
#include <math.h>
#include <rtdbg.h>

float _motor_cal_max_v(float limit_v, float v0, float vt, float fmacc, float fmdac, float x, float delta_t)
{
    float fma = 0;
    float max_v = 0;
    float a = 0;
    float min_v = 0;

    if (v0 < vt)
    {
        fma = fmacc;
    }
    else
    {
        fma = -fmdac;
    }
    float p = (vt * vt - v0 * v0) / (2.f * fma);
    p=fabs(p);
    if (p >= x)
    {
        // 纯减速
        // 计算a

        // a = (vt * vt - v0 * v0) / (2 * x); // 带符号
        // if(a>fmacc)a=fmacc;
        // if(a<-fmdac)a=-fmdac;
        max_v = v0 - fmdac * delta_t;
        max_v = fabs(max_v);
        if (max_v < min_v)
            max_v = 0;
        // LOG_D("max_v:%f v0:%f a:%f", max_v,v0,a);
    }
    else
    {
        // 有加速，有减速,现在必定加速
        max_v = v0 + fmacc * delta_t;

        if((vt * vt - max_v * max_v) / (2.f * fma)){
            /*经过测试还是减速*/
        }else{
            /*加速*/
        }
        if (max_v > limit_v)
            max_v = limit_v;
        if (max_v < min_v)
            max_v = min_v;
    }
   LOG_D("max_v v0 deltav a:%f,%f,%f", max_v,v0,fmacc * delta_t);

    return max_v;
}

float motor_cal_max_v(float limit_v, float v0, float vt, float fmacc, float fmdac, float x, float delta_t)
{
    if (x < 0)
    {
        v0 = -v0;
        vt = -vt;
        x = -x;
        return -_motor_cal_max_v(limit_v, v0, vt, fmacc, fmdac, x, delta_t);
    }
    return _motor_cal_max_v(limit_v, v0, vt, fmacc, fmdac, x, delta_t);
}

rt_weak int32_t motor_get_time(void)
{
    return rt_tick_get();
}
void motor_get_start_state(motor_plannning_t *plan)
{
    if (plan)
    {
        plan->start_point.x = plan->motor->cur_pos;
        plan->start_point.speed = plan->motor->cur_speed;
        plan->start_point.time = motor_get_time();
    }
}
void motor_get_now_state(motor_plannning_t *plan)
{
    if (plan)
    {
        plan->now_point.x = plan->motor->cur_pos;
        plan->now_point.speed = plan->motor->cur_speed;
        plan->now_point.time = motor_get_time();
    }
}

void motor_get_end_state(motor_plannning_t *plan)
{
    if (plan)
    {
        plan->end_point.x = plan->motor->tar_pos/10;
        plan->end_point.speed = 0;
        // plan->end_point.time = motor_get_time();
    }
}

void motor_start_plan(motor_plannning_t *plan)
{
    motor_get_start_state(plan);
    motor_get_end_state(plan);
    motor_get_now_state(plan);
    if (plan)
    {
        motor_planning(plan);
    }
}

void motor_planning(motor_plannning_t *plan)
{
    if (plan)
    {
        motor_get_now_state(plan);
        plan->out_limit_v = fabs(motor_cal_max_v(plan->fm_v, plan->now_point.speed, plan->end_point.speed, plan->fmacc, plan->fmdac, plan->end_point.x - plan->now_point.x, plan->delta_time));
        //motor_set_speed(plan->motor->id, plan->out_limit_v);
        motor_set_speed_limit(plan->motor->id, plan->out_limit_v);
    }
}

void motor_planning_init(motor_plannning_t *plan, motor_t *motor, float fm_v, float fmacc, float fmdac, float delta_time)
{
    if (plan)
    {
        plan->motor = motor;
        plan->fm_v = fm_v;
        plan->fmacc = fmacc;
        plan->fmdac = fmdac;
        plan->delta_time = delta_time;
    }
}

int motor_plan_set_pos(motor_plannning_t *plan, float pos)
{
    if (plan)
    {
        motor_set_pos(0, pos*10);
        motor_start_plan(plan);
        return 0;
    }
    return -1;
}
