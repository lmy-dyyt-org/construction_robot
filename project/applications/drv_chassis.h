#ifndef __DRV_CHASSIS_H__
#define __DRV_CHASSIS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_gpio.h>
#include <ulog.h>
#ifdef __cplusplus
extern "C" {
#endif

#define WHELL_R_M 0.04
#define CHSSIS_A_M 0.218
#define CHSSIS_B_M 0.24

typedef float CSS_MAI_TYPE;
typedef struct{
    CSS_MAI_TYPE wheel_0_offset_speed_m_s;//车轮速度偏移 单位m/s
    CSS_MAI_TYPE wheel_1_offset_speed_m_s;//车轮速度偏移 单位m/s
    CSS_MAI_TYPE wheel_2_offset_speed_m_s;//车轮速度偏移 单位m/s
    CSS_MAI_TYPE wheel_3_offset_speed_m_s;//车轮速度偏移 单位m/s

    CSS_MAI_TYPE wheel_0_offset_pos_m;//车轮位置偏移 单位m
    CSS_MAI_TYPE wheel_1_offset_pos_m;//车轮位置偏移 单位m
    CSS_MAI_TYPE wheel_2_offset_pos_m;//车轮位置偏移 单位m
    CSS_MAI_TYPE wheel_3_offset_pos_m;//车轮位置偏移 单位m
}chassis_mai_offset;
typedef struct{
    CSS_MAI_TYPE xSpeed_m_s;//x速度 单位m/s
    CSS_MAI_TYPE ySpeed_m_s;//y速度 单位m/s
    CSS_MAI_TYPE wSpeed_rad_s;//自旋速度 单位rad/s

    CSS_MAI_TYPE x_m;//x位置 单位m
    CSS_MAI_TYPE y_m;//y位置 单位m
    CSS_MAI_TYPE yaw_rad;//自旋位置 单位弧度
}chassis_mai_state;

typedef struct chassis_mai{

    chassis_mai_state state;
    /**
     *          车轮半径r
     * 
     *          x y轴定义 车轮旋转正方向 电机编号 确定
     * 
     *       // 四轮麦克纳姆轮底盘    采用abab  x型
     * 
     *                    y
     *                    ^          a轴
     *       1轮  \\      |      //  0轮
     *             \\     |     //
     *                    |  
     *                    |_ _ _ _ _ _ _ _ _ > x
     *                  
     *            //           \\
     *      2轮  //             \\  3轮
     *                              b轴
     */
    CSS_MAI_TYPE wheel_r_m;//车轮子半径单位m   0.04

    //对于omni4车辆来说，a与b应该是相同的，故现在代码仅使用a代替b a为车轮转轴中心间距
    CSS_MAI_TYPE chssis_a_m;//车x方向直径单位m  0.218
    CSS_MAI_TYPE chssis_b_m;//车y方向直径单位m  0.24

    CSS_MAI_TYPE wheel_0_speed_m_s;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_1_speed_m_s;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_2_speed_m_s;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_3_speed_m_s;//车轮速度 单位rmp

    CSS_MAI_TYPE wheel_0_pos_rad;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_1_pos_rad;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_2_pos_rad;//车轮速度 单位rmp
    CSS_MAI_TYPE wheel_3_pos_rad;//车轮速度 单位rmp
 
    void(*user_handle)(struct chassis_mai* mai);
}chassis_mai_t;

void drv_chassis(void *parameter);
void chassis_mai_userdata(chassis_mai_t * mai);
void chassis_mai_set_speed(chassis_mai_t * mai, chassis_mai_state* state, chassis_mai_offset* offset);
void chassis_mai_set_position(chassis_mai_t * mai,chassis_mai_state* state, chassis_mai_offset* offset);
void chassis_mai_set_zero_pos(chassis_mai_t * mai);
void chassis_mai_get_state(chassis_mai_t * mai, chassis_mai_state* state);
void chassis_mai_handler(chassis_mai_t * mai,uint32_t time_ms);
int chassis_mai_init(chassis_mai_t * mai,void(*user_handle)(chassis_mai_t* mai));

#ifdef __cplusplus
}
#endif

#endif
