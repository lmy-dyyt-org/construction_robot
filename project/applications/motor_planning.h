#ifndef MOTOR_PLANNING_H
#define MOTOR_PLANNING_H

#ifdef __cplusplus
extern "C" {
#endif
 struct motor_planning;
	typedef  struct motor_planning motor_plannning_t;
#include "motor.h"

typedef struct{
    float x;//开始位置
    float speed;
    // float acc;
    float time;
}point_t;
 struct motor_planning{
    point_t start_point;
    point_t now_point;
    point_t end_point;
    float delta_time;
    float fmacc;
    float fmdac;
    float fm_v;
    float out_limit_v;
    motor_t* motor;
};

void motor_planning_init(motor_plannning_t *plan, motor_t *motor, float fm_v, float fmacc, float fmdac, float delta_time);
void motor_start_plan(motor_plannning_t *plan);
void motor_planning(motor_plannning_t *plan);

int motor_plan_set_pos(motor_plannning_t *plan,float pos);
#ifdef __cplusplus
};
#endif
#endif
