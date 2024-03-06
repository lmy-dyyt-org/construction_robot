#ifndef __DRV_STEPPER_MOTOR_H__
#define __DRV_STEPPER_MOTOR_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_gpio.h>
#include "Emm_V5.h"

#define BIG_ARM_ANGLE_MAX 672.3
#define SMALL_ARM_ANGLE_MAX 672.3

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stepper_motor
{
	uint8_t stepper_motor_id;		/* 电机ID */
	uint8_t stepper_motor_target_dir;		/* 电机旋转方向 */
	uint32_t stepper_motor_target_angle;  	/* 电机目标位置角度 */ //3200个脉冲一圈
	uint16_t stepper_motor_target_speed;  	/* 电机目标转速 */

	uint16_t stepper_motor_speed;   			/* 电机实时转速 */
	float stepper_motor_angle;  			/* 电机实时位置角度 */
	float stepper_motor_err;  			/* 电机位置误差角度 */
	
	uint8_t stepper_motor_reachflag;  			/* 到位状态标志位 */
	uint8_t stepper_motor_stallflag;  			/* 堵转状态标志位 */
	uint8_t stepper_motor_enflag;  				/* 使能状态标志位 */

	uint8_t stepper_motor_encokflag;      			/* 编码器就绪标志位 */
	uint8_t stepper_motor_calibrationflag;			/* 校准就绪标志位 */
	uint8_t stepper_motor_returnzeroingflag;		/* 正在回零状态标志位 */
	uint8_t stepper_motor_returnzero_failflag; 		/* 回零失败状态标志位 */
}stepper_motor_t;

extern uint8_t rxCmd[128];

void drv_stepper_motor(void *parameter);

rt_device_t Emm_V5_Init(const char* uart);    
void Emm_V5_Transmit(uint8_t* data, uint8_t len);
int Emm_V5_Receive(uint8_t* data, uint8_t len);

void stepper_motor_Init(stepper_motor_t* stepper_motor, uint8_t id);

#ifdef __cplusplus
}
#endif

#endif
