/**
 * @file motor.h
 * @author Dyyt587 (805207319@qq.com)
 * @brief 电机通用驱动
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MOTOR_H
#define MOTOR_H
#if defined(__cplusplus)
extern "C" {
#endif
#include "pid.h"

/* Compiler Related Definitions */
#include "rtcompiler.h"



#define MOTOR_USING_AUTO_INIT
#define MOTOR_DEBUGING_AUTO_INIT







typedef enum{

    MOTOR_MODE_TORQUE= 1U,
    MOTOR_MODE_SPEED = 2U,
    MOTOR_MODE_POS   = 3U,
    MOTOR_MODE_VOLTAGE,
    MOTOR_MODE_TEMP,
    MOTOR_MODE_MAX,

    MOTOR_CONTROL_SUPPORT_TORQUE = 0,   //硬件支持
    MOTOR_CONTROL_SUPPORT_SPEED = 1,    //速度支持
    MOTOR_CONTROL_SUPPORT_POS = 2,      //位置支持


}MOTOR_VALUE_TYPE;

typedef enum{

    MOTOR_CONTROL_SUPPORT_TORQUE = 1,   //硬件支持
    MOTOR_CONTROL_SUPPORT_SPEED = 1<<1,    //速度支持
    MOTOR_CONTROL_SUPPORT_POS = 1<<2,      //位置支持

};
typedef int(*motor_driver)(MOTOR_VALUE_TYPE mode,void* value, void* user_data);
typedef int(*motor_ctr)(MOTOR_VALUE_TYPE mode,void*data, void* user_data);
typedef int(*motor_behiver)(MOTOR_VALUE_TYPE mode,void*data, void* user_data);
typedef struct{
    motor_driver driver;
    motor_ctr control;
    void* user_data;
}motor_ops_t;
typedef struct{
    motor_ops_t* ops;
    int id;
    const char* name;
    PID_T* pid_speed;
    PID_T* pid_pos;
    PID_T* pid_torque;

    float tar_speed;
    float tar_pos;
    float tar_torque;

    float cur_speed;
    float cur_pos;
    float cur_torque;
    int flag_run_mode :2;//记录当前运行模式
    int flag_accept_mode :3;//记录支持的模式 0 表示数据直接输出 
    motor_behiver behaver;
}motor_t;



/* initialization export */
#ifdef MOTOR_USING_AUTO_INIT
typedef int (*init_fn_t)(void);
#ifdef _MSC_VER
#pragma section("__motor_$f",read)
    #ifdef MOTOR_DEBUGING_AUTO_INIT
        struct motor_init_desc
        {
            const char* level;
            motor_t* motor;
            const char* fn_name;
        };
        #define INIT_EXPORT(fn, level)                                  \
                                const char __motor_level_##fn[] = ".__motor_." level;       \
                                const char __motor_##fn##_name[] = #fn;                   \
                                __declspec(allocate("__motor_$f"))                        \
                                rt_used const struct motor_init_desc __rt_init_msc_##fn =  \
                                {__motor_level_##fn, fn, __motor_##fn##_name};
    #else
        struct motor_init_desc
        {
            const char* level;
            motor_t* motor;
        };
        #define INIT_EXPORT(fn, level)                                  \
                                const char __motor_level_##fn[] = ".__motor_." level;       \
                                __declspec(allocate("__motor_$f"))                        \
                                rt_used const struct motor_init_desc __rt_init_msc_##fn =  \
                                {__motor_level_##fn, fn };
    #endif /* MOTOR_DEBUGING_AUTO_INIT */
#else
    #ifdef MOTOR_DEBUGING_AUTO_INIT
        struct motor_init_desc
        {
            const char* fn_name;
            motor_t* motor;
        };
        #define INIT_EXPORT(fn, level)                                                       \
            const char __motori_##fn##_name[] = #fn;                                            \
            rt_used const struct motor_init_desc __motor_init_desc_##fn rt_section(".__motor_." level) = \
            { __motori_##fn##_name, fn};
    #else
        #define INIT_EXPORT(fn, level)                                                       \
            rt_used const motor_t* __motor_init_##fn rt_section(".__motor_." level) = fn
    #endif /* RT_DEBUGING_AUTO_INIT */
#endif /* _MSC_VER */
#else
#define MOTOR_INIT_EXPORT(motor, id)
#endif /* RT_USING_COMPONENTS_INIT */



int motor_get_id(const char* name);
int motor_create(motor_ops_t*ops);
int motor_handle(int id,int cycle);

int motor_set_speed(int id, float value);

int motor_set_pos(int id, float value);

int motor_set_torque(int id, float value);


int motor_get_speed(int id, float* value);\

int motor_get_pos(int id, float* value);

int motor_get_torque(int id, float* value);

int motor_control(int id, MOTOR_VALUE_TYPE mode,void*data);

#if defined(__cplusplus)
}
#endif
#endif