#include "drv_stepper_motor.h"

stepper_motor_t stepper_motor_1;

float pos = 0.0f, Motor_Cur_Pos = 0.0f;
int state;

static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */
int Emm_rx_flag = 0;                         /* 串口接收标志 */

void drv_stepper_motor(void *parameter)
{

  Emm_V5_Init("uart8");
  stepper_motor_Init(&stepper_motor_1, 1);
  while(1)
  {
    /* 线程处理 */
    Emm_V5_Pos_Control(1, 2, 100, 0, 3200, 0, 0);//位置模式：方向CW，速度1000RPM，加速度0（不使用加减速直接启动），脉冲数3200（16细分下发送3200个脉冲电机转一圈），相对运动
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
      stepper_motor_1.stepper_motor_angle = (float)pos * 360.0f / 65536.0f;

      // 符号
      if(rxCmd[2])
      { stepper_motor_1.stepper_motor_angle = -stepper_motor_1.stepper_motor_angle; }
	  }

		/* 线程运行，打印计数 */

  	rt_kprintf("receive_state: %d\n", state);
	  rt_kprintf("Motor_Cur_Pos: %f\n", stepper_motor_1.stepper_motor_angle);
	  rt_kprintf("Emm_rx_flag: %d\n", Emm_rx_flag);

    rt_thread_mdelay(500);
  }
}

	
uint8_t rxCmd[128] = {0};
uint8_t rxCount = 0;

rt_device_t Emm_serial1; 

static rt_err_t Emm_uart_receive_callback(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    Emm_rx_flag = 1;
    
    rt_sem_release(&rx_sem);
    
    return RT_EOK;
}

rt_device_t Emm_V5_Init(const char* uart)
{
  Emm_serial1 = rt_device_find(uart);
  rt_device_open(Emm_serial1, RT_DEVICE_FLAG_RX_NON_BLOCKING | RT_DEVICE_FLAG_TX_BLOCKING);
  /* 初始化信号量 */
  rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
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
  stepper_motor->stepper_motor_target_dir = 0;
  stepper_motor->stepper_motor_target_angle = 0;
  stepper_motor->stepper_motor_target_speed = 0;
  stepper_motor->stepper_motor_speed = 0;
  stepper_motor->stepper_motor_angle = 0;
  stepper_motor->stepper_motor_err = 0;
  stepper_motor->stepper_motor_enflag = 0;
  stepper_motor->stepper_motor_zeroflag = 0;
  Emm_V5_Reset_CurPos_To_Zero(id);
}
