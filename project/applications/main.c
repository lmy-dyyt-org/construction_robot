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
/* defined the LED_G pin: PF14 */
#define LED0_PIN    GET_PIN(F, 14)


#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     18           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */


#define THREAD_PRIORITY    25
#define THREAD_STACK_SIZE  512
#define THREAD_TIMESLICE    5


float pos = 0.0f, Motor_Cur_Pos = 0.0f;
int state;///////////////////////////6666666666666666666666666666666666
int main(void)
{

///////////////////////////////////////////////////////////创建线程
	  rt_thread_t tid = RT_NULL;
  
    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
  tid = rt_thread_create("follow_line",
              follow_line, RT_NULL,
              THREAD_STACK_SIZE,
              THREAD_PRIORITY, THREAD_TIMESLICE);
              
  /* 线程创建成功，则启动线程 */
  if (tid != RT_NULL)
  {
    rt_thread_startup(tid);
  }
//////////////////////////////////////////////////////////////
	
	
	    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

	char buf[]= "fgweuygfuyweg";
	LOG_HEX("test",16,buf,sizeof(buf));



// 	rt_adc_device_t adc_dev;
// 	rt_uint32_t value;
// float 	vol;
// 	rt_err_t ret = RT_EOK;
// 	rt_uint8_t count =0;
	//rt_thread_mdelay(2000);
	
	//Emm_V5_Init("uart8");
	
	// Emm_V5_Vel_Control(1, 0, 1000, 10, 0); //速度模式

    while (1)
    {
	/* 查找设备 */
	// adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
	// if(adc_dev == RT_NULL)
	// {
	// 	rt_kprintf("adc smaple run failed! can't find %s device!\n", ADC_DEV_NAME);
	// 	return RT_ERROR;
	// }
	
	  Emm_V5_Pos_Control(1, 0, 1000, 0, 10000, 0, 0);//位置模式：方向CW，速度1000RPM，加速度0（不使用加减速直接启动），脉冲数3200（16细分下发送3200个脉冲电机转一圈），相对运动
	rt_thread_mdelay(2000);
	Emm_V5_Read_Sys_Params(1, S_CPOS);
	while(Emm_rx_flag==1)
	{
		state = Emm_V5_Receive(rxCmd, 12);
		Emm_rx_flag = 0;
	}
	if(rxCmd[0] == 1 && rxCmd[1] == 0x36  )//&& rxCount == 8
 	{
   // 拼接成uint32_t类型
   pos = (uint32_t)(
                     ((uint32_t)rxCmd[3] << 24)    |
                     ((uint32_t)rxCmd[4] << 16)    |
                     ((uint32_t)rxCmd[5] << 8)     |
                     ((uint32_t)rxCmd[6] << 0)
                   );
   // 转换成角度
   Motor_Cur_Pos = (float)pos * 360.0f / 65536.0f;

   // 符号
   if(rxCmd[2])
	 { Motor_Cur_Pos = -Motor_Cur_Pos; }
	}
	rt_kprintf("receive_state: %d\n", state);
	rt_kprintf("Motor_Cur_Pos: %f\n", Motor_Cur_Pos);
	rt_kprintf("Emm_rx_flag: %d\n", Emm_rx_flag);





	/* 使能设备 */





	// ret  = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
	
	// while(count <10)
   	// 	count ++;
	// 	/* 读取采集值 */
	// 	value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
	// 	rt_kprintf("the value is :%d \n", value);
 
	// 	/* 转换为对应电压值 */
	// 	vol = (float)(value * 3.3f) / CONVERT_BITS;
	// 	rt_kprintf("the voltage is :%f \n", vol);		
	// 	float tt = (1.43 - vol)/0.0043 + 25;   //根据公式算出温度值
	// LOG_D("VSense:%.2f\r\n",tt);

	// 	rt_thread_mdelay(500);
	


        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
