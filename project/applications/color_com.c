#include <rtdbg.h>
#define DBG_TAG              "color.com"
#define DBG_LVL               DBG_INFO
#include "rtthread.h"
#include <rtdevice.h>
#include "math.h"
#include "board.h"
#include "aboard_power_switch.h"
uint8_t color[128]={0};
rt_device_t uart=0;
uint8_t color_flag=0;
// extern rt_uint8_t color_type;

// enum
// {
//     RED,
//     BLUE,
//     YELLOW,
// 	NONE,
// };

rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
		// while(size--){
					rt_device_read(uart, 0, &color, size);	
		 //}
		 switch (color[0])
		 {
		 case 114:
			 	rt_pin_write(GET_PIN(G,1),0);
				rt_pin_write(GET_PIN(G,3),1);
				rt_pin_write(GET_PIN(G,5),1);
				rt_pin_write(GET_PIN(G,7),1);
		 	//color_type = RED; 
		 //rt_pin_write(GET_PIN(A,1),0);
		 	break;
		 case 121:
//		 	color_type = YELLOW;
			 	rt_pin_write(GET_PIN(G,1),1);
				rt_pin_write(GET_PIN(G,3),1);
				rt_pin_write(GET_PIN(G,5),0);
				rt_pin_write(GET_PIN(G,7),1);	 
		 	break;
		 case 98:
//		 	color_type = BLUE;
			 	rt_pin_write(GET_PIN(G,1),1);
				rt_pin_write(GET_PIN(G,3),0);
				rt_pin_write(GET_PIN(G,5),1);
				rt_pin_write(GET_PIN(G,7),1);
		 	break;
		 case 120:
//		 	color_type = NONE;
			 	rt_pin_write(GET_PIN(G,1),1);
				rt_pin_write(GET_PIN(G,3),1);
				rt_pin_write(GET_PIN(G,5),1);
				rt_pin_write(GET_PIN(G,7),1);
		 	break;								
		 default:
		 	//LOG_E("color error!");
//		 	color_type = NONE;
			 	rt_pin_write(GET_PIN(G,1),1);
				rt_pin_write(GET_PIN(G,3),1);
				rt_pin_write(GET_PIN(G,5),1);
				rt_pin_write(GET_PIN(G,7),1);
		 	break;
		 }
		
		color_flag=1;
		// LOG_D("color:%d",color_type);
		return RT_EOK;
}

void color_com(void *parameter)
{
	rt_pin_mode(GET_PIN(G,1),OUTPUT_PP);
	rt_pin_mode(GET_PIN(G,3),OUTPUT_PP);
	rt_pin_mode(GET_PIN(G,5),OUTPUT_PP);
	rt_pin_mode(GET_PIN(G,7),OUTPUT_PP);
	uart = rt_device_find("uart7");
	if (uart == RT_NULL)
	{
		LOG_E("uart7 not found");
		return;
	}
	if (rt_device_open(uart, RT_DEVICE_FLAG_RX_NON_BLOCKING | RT_DEVICE_FLAG_TX_BLOCKING) != RT_EOK)
	{
		LOG_E("uart7 open failed");
		return;
	}
		if (rt_device_set_rx_indicate(uart, uart_rx_ind) != RT_EOK)
	{
		LOG_E("uart7 set rx indicate failed");
		return;
	}
	while(1)
	{
		// rt_device_write(uart, 0, &color, 1);
		
		rt_thread_mdelay(100);
	}
}


int color_com_init(void)
{
	  // 创建电机信息处理线程
#define THREAD_PRIORITY_PROCESS_STEPPER_MOTOR    8
#define THREAD_STACK_SIZE_PROCESS_STEPPER_MOTOR  1024
#define THREAD_TIMESLICE_PROCESS_STEPPER_MOTOR    5
		 rt_thread_t color = RT_NULL;
  
  color = rt_thread_create("color_com",
              color_com, RT_NULL,
              THREAD_STACK_SIZE_PROCESS_STEPPER_MOTOR,
              THREAD_PRIORITY_PROCESS_STEPPER_MOTOR, THREAD_TIMESLICE_PROCESS_STEPPER_MOTOR);

  if (color != RT_NULL)
  {
    rt_thread_startup(color);
  }
	return 0;
}INIT_APP_EXPORT(color_com_init);

