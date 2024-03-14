/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-06 16:16:14
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-15 01:46:20
 * @FilePath: \project\applications\drv_stepper_motor.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "rtthread.h"
#include "drv_stepper_motor.h"

stepper_motor_t stepper_motor_1;

static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */
uint8_t rxCmd[128] = {0};
rt_device_t Emm_serial1; 

void drv_stepper_motor(void *parameter)
{
	/*通信初始化 电机初始化*/
  Emm_V5_Init("uart7");
  stepper_motor_Init(&stepper_motor_1, 1);
	
	/*清零电机位置*/
  // Emm_V5_Reset_CurPos_To_Zero(1);//01 0A 02 6B
  // Emm_V5_En_Control(1, 1, 0);
  //Emm_V5_Origin_Modify_Params(1, 1, 2, 0, 30, 5000, 300, 400, 60, 0);//参数4是方向 倒数第三个参数是电流值    这是控制小臂的电机（多一个件的一边）  方向0 是控制往上抬

  Emm_V5_Origin_Modify_Params(1, 1, 2, 1, 30, 5000, 300, 350, 60, 0);//参数4是方向   倒数第三个参数是电流值   这是控制大臂的电机（螺丝很长的一边）  方向1 是控制往上抬

  rt_thread_mdelay(50);
  Emm_V5_Origin_Trigger_Return(1, 2, 0);

  LOG_D("stepper_motor_1 init success");
	while(1)
  {
    //Emm_V5_Pos_Control(1, 0, 100, 0, 3200, 0, 0);//01 fd 02 6b	
		// Emm_V5_Vel_Control(1, 0, 100, 0, 0); 
  LOG_D("stepper_motor_1 ");
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
int stepper_motor_init(void)
{
	#define THREAD_PRIORITY_STEPPER_MOTOR    25
#define THREAD_STACK_SIZE_STEPPER_MOTOR  1024
#define THREAD_TIMESLICE_STEPPER_MOTOR    5
		 rt_thread_t tid_stepper_motor = RT_NULL;
  
    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
  tid_stepper_motor = rt_thread_create("drv_stepper_motor",
              drv_stepper_motor, RT_NULL,
              THREAD_STACK_SIZE_STEPPER_MOTOR,
              THREAD_PRIORITY_STEPPER_MOTOR, THREAD_TIMESLICE_STEPPER_MOTOR);
              
  /* 线程创建成功，则启动线程 */
  if (tid_stepper_motor != RT_NULL)
  {
    rt_thread_startup(tid_stepper_motor);
  }
	return 0;
}INIT_DEVICE_EXPORT(stepper_motor_init);
