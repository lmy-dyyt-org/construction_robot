/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2023-12-03     Meco Man     support nano version
 */

#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "ulog.h"
#include "Emm_V5.h"
#include "follow_line.h"
#include "drv_stepper_motor.h"
#include "aboard_power_switch.h"
#include "motor.h"
#include "motor_planning.h"
#include "apid_auto_tune_ZNmode.h"
/* defined the LED_G pin: PF14 */
#define LED0_PIN GET_PIN(F, 14)

#define ADC_DEV_NAME "adc1"	   /* ADC 设备名称 */
#define ADC_DEV_CHANNEL 18	   /* ADC 通道 */
#define REFER_VOLTAGE 330	   /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS (1 << 12) /* 转换位数为12位 */

#define THREAD_PRIORITY_FOLLOW_LINE 25
#define THREAD_STACK_SIZE_FOLLOW_LINE 1024
#define THREAD_TIMESLICE_FOLLOW_LINE 5

#define THREAD_PRIORITY_CHASSIS 25
#define THREAD_STACK_SIZE_CHASSIS 1024
#define THREAD_TIMESLICE_CHASSIS 5
apid_auto_tune_ZNmode_t tuner;
int main(void)
{
	// //////////////////////////////////////////////////////////////创建红外循迹线程
	// 	  rt_thread_t tid_follow_line = RT_NULL;

	//     /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
	//   tid_follow_line = rt_thread_create("follow_line",
	//               follow_line, RT_NULL,
	//               THREAD_STACK_SIZE_FOLLOW_LINE,
	//               THREAD_PRIORITY_FOLLOW_LINE, THREAD_TIMESLICE_FOLLOW_LINE);

	//   /* 线程创建成功，则启动线程 */
	//   if (tid_follow_line != RT_NULL)
	//   {
	//     rt_thread_startup(tid_follow_line);
	//   }
	//////////////////////////////////////////////////////////////创建步进电机线程

	//////////////////////////////////////////////////////////////创建底盘运动线程

	//////////////////////////////////////////////////////////////
	/* set LED0 pin mode to output */
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

	power_on(SWITCH_24V_1);
	power_on(SWITCH_24V_2);
	power_off(SWITCH_24V_3);
	power_on(SWITCH_24V_4);
	power_on(SWITCH_5V_1);

	power_off(SWITCH_24V_4);

	// 	rt_adc_device_t adc_dev;
	// 	rt_uint32_t value;
	// float 	vol;
	// 	rt_err_t ret = RT_EOK;
	// 	rt_uint8_t count =0;
	// rt_thread_mdelay(2000);
	static motor_plannning_t plan;
	motor_t *motor = motor_get(0);
	// motor_set_speed(0, 100);
	//  #define N 500
	// motor_planning_init(&plan,motor,200,200,200,0.1);
	// motor_plan_set_pos(&plan,1800*2);

	// apid_auto_tune_ZNmode_init(&tuner, 5, 5, 100, 0, 0, ZNModeLessOvershoot);
	// setOutputRange(&tuner, -20000, 20000);
	// startTuningLoop(&tuner);
	// while (!isFinished(&tuner))
	// {
	// 	// APID_Pause(motor->pid_pos);
	// 	// APID_Pause(motor->pid_speed);
	// 	//        // This loop must run at the same speed as the PID control loop being tuned
	// 	//        long prevMicroseconds = microseconds;
	// 	//        microseconds = micros();

	// 	// Get input value here (temperature, encoder position, velocity, etc)
	// 	float input = motor_get_speed(0);

	// 	// Call tunePID() with the input value and current time in microseconds
	// 	float output = tunePID(&tuner, input, 5);

	// 	// Set the output - tunePid() will return values within the range configured
	// 	// by setOutputRange(). Don't change the value or the tuning results will be
	// 	// incorrect.
	// 	// doSomethingToSetOutput(output);
	// 	// tuner.i++;
	// 	static int cnt = 0;
	// 	if (cnt++ % 10 == 0)
	// 		LOG_D("in out:%f,%f,%d", input, output, tuner.i);
	// 	motor_set_torque(0, output);

	// 	// motor->ops->driver(0,MOTOR_MODE_TORQUE,&output,motor->ops->user_data);
	// 	// This loop must run at the same speed as the PID control loop being tuned
	// 	// while (micros() - microseconds < loopInterval) delayMicroseconds(1);
	// 	rt_thread_mdelay(5);
	// }
	// motor_set_torque(0, 0);
	 motor_set_speed(0, 0);
	// LOG_D("kp:%f ki:%f kd:%f", getKp(&tuner), getKi(&tuner), getKd(&tuner));

	while (1)
	{
		// motor_set_pos(0,2000);
		// motor_planning(&plan);

		rt_pin_write(LED0_PIN, PIN_HIGH);
		rt_thread_mdelay(500);
		rt_pin_write(LED0_PIN, PIN_LOW);
		rt_thread_mdelay(500);
	}
}
