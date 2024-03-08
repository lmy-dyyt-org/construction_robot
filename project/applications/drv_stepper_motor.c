/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-06 16:16:14
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-07 11:46:13
 * @FilePath: \project\applications\drv_stepper_motor.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "drv_stepper_motor.h"

stepper_motor_t stepper_motor_1;

static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */
uint8_t rxCmd[128] = {0};
rt_device_t Emm_serial1; 

void drv_stepper_motor(void *parameter)
{
	/*通信初始化 电机初始化*/
  Emm_V5_Init("uart8");
  stepper_motor_Init(&stepper_motor_1, 1);
	
	/*清零电机位置*/
  // Emm_V5_Reset_CurPos_To_Zero(1);//01 0A 02 6B
  // Emm_V5_En_Control(1, 1, 0);
  Emm_V5_Origin_Modify_Params(1, 1, 2, 0, 30, 10000, 300, 70, 60, 0);
  rt_thread_mdelay(50);
  Emm_V5_Origin_Trigger_Return(1, 2, 0);
	while(1)
  {
    //Emm_V5_Pos_Control(1, 0, 100, 0, 3200, 0, 0);//01 fd 02 6b	
		// Emm_V5_Vel_Control(1, 0, 100, 0, 0); 

    rt_thread_mdelay(600);//这里的延时要根据 速度 和 转动圈数来取
		
    Emm_V5_Read_Sys_Params(&stepper_motor_1, 1, S_CPOS);
    Emm_V5_Read_Sys_Params(&stepper_motor_1, 1, S_VEL);
    Emm_V5_Read_Sys_Params(&stepper_motor_1, 1, S_PERR);
    Emm_V5_Read_Sys_Params(&stepper_motor_1, 1, S_FLAG);
    Emm_V5_Read_Sys_Params(&stepper_motor_1, 1, S_ORG);
    rt_thread_mdelay(500);
  }
}

/* 接收数据回调函数 */
static rt_err_t Emm_uart_receive_callback(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    
    rt_sem_release(&rx_sem);
    
    return RT_EOK;
}

rt_device_t Emm_V5_Init(const char* uart)
{
  /* 查找系统中的串口设备 */
  Emm_serial1 = rt_device_find(uart);
  /* 初始化信号量 */
  rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
  /* 以阻塞接收及轮询发送模式打开串口设备 */
  rt_device_open(Emm_serial1, RT_DEVICE_FLAG_RX_BLOCKING | RT_DEVICE_FLAG_TX_BLOCKING);
  /* 设置接收回调函数 */
  rt_device_set_rx_indicate(Emm_serial1, Emm_uart_receive_callback);  
  
	return Emm_serial1;
}

void Emm_V5_Transmit(uint8_t* data, uint8_t len)
{
  rt_device_write(Emm_serial1, 0, data, len);
}

int Emm_V5_Receive(uint8_t* data, uint8_t len)
{
  return rt_device_read(Emm_serial1, 0, data, len);
}

void stepper_motor_Init(stepper_motor_t* stepper_motor, uint8_t id)
{
  stepper_motor->stepper_motor_id = id;
}
