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

#include "Trajectory_planning.h"

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


/////////////////////////////////////////////////////////////////////////////////////////
//使用示例
	 motor_set_pos(0, 0);
		CurveObjectType curve;
		
		
		curve.targetPosm = 0.4f;//m
		
    curve.startPos = 0.0f;     // 初始位置
    curve.currentPos = 0.0f;   
    curve.targetPos = curve.targetPosm * 360.f/0.2198f; // 目标位置
    curve.stepPos = 0.1f;     // 位置变化的步长
	curve.max_pos = 1000.0f;
    curve.PosMax = curve.max_pos;    // 最大位置限制
    curve.PosMin = -curve.max_pos;      // 最小位置限制
    curve.aTimes = 0;            // 当前时间步
    curve.maxTimes = 500;          // 总时间步，实际使用时需要根据实际情况计算
    curve.curveMode = CURVE_SPTA; // 使用S型曲线
    curve.flexible = 10.f;       // S曲线的柔性因子


    for (int i = 0; i < curve.maxTimes; ++i)
    {
				rt_thread_mdelay(10);
        mine_plan(&curve);
			//LOG_D("Current Pos:%f\n", curve.currentPos);
				 motor_set_pos(0, curve.currentPos);
    }
////////////////////////////////////////////////////////////////////////////////////////


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


