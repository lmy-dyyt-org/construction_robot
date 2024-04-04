#include "robot_move_direct.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

#define DBG_TAG "robot_move_direct"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>



void Robot_Move_Direct_Process(void *parameter)
{
    while(1)
    {
        rt_thread_delay(10);
    }
}

int Robot_Move_Direct_Init(void)
{
	rt_thread_t fsus_thread = RT_NULL;
	// // 创建信号量
	// rt_sem_init(&FSUS_sem1, "fsus_sem", 0, RT_IPC_FLAG_FIFO);
	// servo1.rx_sem = &FSUS_sem1;
	// /* 初始化静态互斥量 */
	// rt_err_t result = rt_mutex_init(&static_fsus_mutex1, "fsus_mutex", RT_IPC_FLAG_FIFO);
	// if (result != RT_EOK)
	// {
	// 	rt_kprintf("init static mutex failed.\n");
	// 	return -1;
	// }
	// servo1.mutex = &static_fsus_mutex1;

	// // 初始化串口
	// servo1.uart = rt_device_find("uart3");
	// if (servo1.uart == RT_NULL)
	// {
	// 	rt_kprintf("Can't find uart device\n");
	// 	return -1;
	// }
	// rt_device_open(servo1.uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	// rt_device_set_rx_indicate(servo1.uart, FSUS_uart_receive_callback1);
	// servo1.rx_size = 0;
    
	// 创建线程
	fsus_thread = rt_thread_create("drv_fsus",
								   Robot_Move_Direct_Process, NULL,
								   1024,
								   15, 1);

	if (fsus_thread != RT_NULL)
	{
		rt_thread_startup(fsus_thread);
	}

	return 0;
}
INIT_APP_EXPORT(Robot_Move_Direct_Init);