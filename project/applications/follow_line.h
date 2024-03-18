/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-02 22:00:25
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-18 18:53:08
 * @FilePath: \project\applications\follow_line.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __FOLLOW_LINE_H__
#define __FOLLOW_LINE_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_gpio.h>
#include "statemachine.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "abus_topic.h"

#define FRONT_INFRARED0_PIN    GET_PIN(C, 4)	//前左0 N1
#define FRONT_INFRARED1_PIN    GET_PIN(C, 3)	//前左1 M1
#define FRONT_INFRARED2_PIN    GET_PIN(C, 2)	//前中0 L1
#define FRONT_INFRARED3_PIN    GET_PIN(E, 6)	//前中1 K1
#define FRONT_INFRARED4_PIN    GET_PIN(E, 5)	//前右0 J1
#define FRONT_INFRARED5_PIN    GET_PIN(F, 1)	//前右1 I1
	
#define LEFT_INFRARED0_PIN    GET_PIN(F, 0)		//左左0 I2
#define LEFT_INFRARED1_PIN    GET_PIN(E, 4)		//左左1 J2
#define LEFT_INFRARED2_PIN    GET_PIN(E, 12)	//左中0 K2
#define LEFT_INFRARED3_PIN    GET_PIN(B, 0)		//左中1 L2
#define LEFT_INFRARED4_PIN    GET_PIN(B, 1)		//左右0 M2
#define LEFT_INFRARED5_PIN    GET_PIN(C, 0)		//左右1 N2

#define RIGHT_INFRARED0_PIN    GET_PIN(C, 5)	//右左0 O1
#define RIGHT_INFRARED1_PIN    GET_PIN(C, 1)	//右左1 O2
#define RIGHT_INFRARED2_PIN    GET_PIN(A, 5)	//右中0 P1
#define RIGHT_INFRARED3_PIN    GET_PIN(A, 4)	//右中1 P2
#define RIGHT_INFRARED4_PIN    GET_PIN(F, 10)	//右右0 Q1
#define RIGHT_INFRARED5_PIN    GET_PIN(I, 9)	//右右1 Q2

enum{
	front_left0_infrared=0,
	front_left1_infrared,
	front_middle0_infrared,
	front_middle1_infrared,
	front_right0_infrared,
	front_right1_infrared,

	left_left0_infrared,
	left_left1_infrared,
	left_middle0_infrared,
	left_middle1_infrared,
	left_right0_infrared,
	left_right1_infrared,

	right_left0_infrared,
	right_left1_infrared,
	right_middle0_infrared,
	right_middle1_infrared,
	right_right0_infrared,
	right_right1_infrared,
};

enum direction{
	front=0,
	left,
	right,
	rotate,
};

typedef struct infrared
{
	rt_uint8_t infrared_data[18];

	uint8_t is_spacial_point_flag;
	enum direction move_direction;
}infrared;


void follow_line(void *parameter);

void Infrared_Init(void);
void GET_Infrared_Data(infrared* infrared_package);
void Print_Infrared_Data(infrared* infrared_package);

uint8_t Is_Spacial_point(infrared* infrared_package);

#ifdef __cplusplus
}
#endif

#endif
