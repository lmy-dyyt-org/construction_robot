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

stepper_motor_t stepper_motor_big_arm;
stepper_motor_t stepper_motor_small_arm;

static struct rt_semaphore rx_sem;    /* 用于接收消息的信号量 */
uint8_t rxCmd[128] = {0};
rt_device_t Emm_serial1; 

/* 邮箱控制块 */
static struct rt_mailbox mb1;
/* 用于放邮件的内存池 */
static char mb_pool1[128];

uint8_t stepper_motor_cmd_state = S_IDLE; //初始化命令接收为空闲

rt_mutex_t  mutex_step = RT_NULL;//互斥锁
rt_sem_t step_sem = RT_NULL;//信号量

void drv_stepper_motor(void *parameter)
{
	/*通信初始化 电机初始化*/
  Emm_V5_Init("uart8");
  stepper_motor_Init(&stepper_motor_big_arm, 1);
	stepper_motor_Init(&stepper_motor_small_arm, 3);
  rt_thread_mdelay(2000); //上电需要等两秒 初始化

  // //回零调参 电机有存储的 没必要每次都重新设置
  // Emm_V5_Origin_Modify_Params(1, 1, 2, 1, 30, 5000, 300, 700, 60, 0);//参数4是方向   倒数第三个参数是电流值   这是控制大臂的电机（螺丝很长的一边）  方向1 是控制往上抬
  // Emm_V5_Origin_Modify_Params(3, 1, 2, 0, 30, 5000, 300, 700, 60, 0);//参数4是方向 倒数第三个参数是电流值    这是控制小臂的电机（多一个件的一边）  第四个参数 方向0 是控制往上抬

  // rt_thread_mdelay(100); //设置参数之后需要延时！！！！！！！！！
  Emm_V5_Origin_Trigger_Return(1, 2, 0);
   Emm_V5_Origin_Trigger_Return(3, 2, 0);

  while(1)
  {
    rt_thread_mdelay(500);
    //Emm_V5_Pos_Control(1, 0, 100, 0, 3200, 0, 0);//01 fd 02 6b	
		// Emm_V5_Vel_Control(1, 0, 100, 0, 0); 
    // rt_thread_mdelay(600);//这里的延时要根据 速度 和 转动圈数来取

    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_CPOS);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_VEL);
    // Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_PERR);
    // Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_FLAG);
    // Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_ORG);

    // Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_CPOS);
    // Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_VEL);
    // Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_PERR);
    // Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_FLAG);
    // Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_ORG);

  }
}

/* 接收数据回调函数 */
static rt_err_t Emm_uart_receive_callback(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    // rt_sem_release(&rx_sem);

    /* 发送 缓冲区大小 到邮箱中 */
   
    switch(stepper_motor_cmd_state)                           // 功能码
    {
      case S_VER  :  break;
      case S_RL   :  break;
      case S_PID  :  break;
      case S_VBUS :  break;
      case S_CPHA :  break;
      case S_ENCL :  break;
      case S_TPOS :  break;
      case S_VEL  :  
      if(size>=6)  
      {
        LOG_D("vel,size:%d\n",size);
        rt_mb_send(&mb1, (uint8_t)size); 
      }
      break;
      case S_CPOS :  
      if(size>=8)  
      {
        LOG_D("pos,size:%d\n",size);
        rt_mb_send(&mb1, (uint8_t)size);
      }
      break;
      case S_PERR :  if(size>=8)  rt_mb_send(&mb1, (uint8_t)size); break;
      case S_FLAG :  if(size>=4)  rt_mb_send(&mb1, (uint8_t)size); break;
      case S_ORG  :  if(size>=4)  rt_mb_send(&mb1, (uint8_t)size); break;
      case S_Conf :  break;
      case S_State:  break;
      case S_IDLE :  LOG_D("idle\n");  rt_mb_send(&mb1, (uint8_t)size); break;
      default: break;
    }
    return RT_EOK;
}

void drv_process_steppermotor(void *parameter)
{
	uint8_t rx_size;
  uint8_t data[64];
  uint8_t tmp[64];
  uint8_t rx_count=0;
  stepper_motor_t* stepper_motor;
  rt_thread_mdelay(2000);
  int8_t t = rt_device_read(Emm_serial1, 0, tmp, 60);
  LOG_D("stepperMotor_Receive_Error: %d\n", t);

  /* 设置接收回调函数 */
  rt_device_set_rx_indicate(Emm_serial1, Emm_uart_receive_callback);  

  while(1)
  {
        rt_sem_take(step_sem, RT_WAITING_FOREVER); //获取通知
        LOG_D("rece_take_sem\n");
        rt_mutex_take(mutex_step, RT_WAITING_FOREVER);  //获取电机处理权

      /* 从邮箱中收取邮件 */
      if (rt_mb_recv(&mb1, &rx_size, RT_WAITING_FOREVER) == RT_EOK)
      {
        LOG_D("start_process,rx_size:%d\n",rx_size);
        // rt_device_read(Emm_serial1, 0, &data, 1);
        // LOG_I("stepperMotor_Receive: %d,%x,%d\n",rx_size,data,++rx_count);
        switch(stepper_motor_cmd_state)                           // 功能码
        {
          case S_IDLE : 
                        LOG_D("stepperMotor_Receive: %x,%d,%d\n",data[0],rt_device_read(Emm_serial1, 0, data, rx_size),++rx_count);//把缓冲区的垃圾清空
                         break;
          case S_VER  :  break;
          case S_RL   :  break;
          case S_PID  :  break;
          case S_VBUS :  break;
          case S_CPHA :  break;
          case S_ENCL :  break;
          case S_TPOS :  break;
          case S_VEL  :  
                        if(rx_size >= 6)
                        {
                            Emm_V5_Receive(rxCmd, 6);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                            if(rxCmd[1] == 0x35)
                            {  
                              stepper_motor->stepper_motor_speed = (uint16_t)(
                                                ((uint16_t)rxCmd[3] << 8)    |
                                                ((uint16_t)rxCmd[4] << 0)    
                                              );
                              // 修正电机正反速度
                              if(rxCmd[2])
                              { stepper_motor->stepper_motor_speed = -stepper_motor->stepper_motor_speed; }
                              stepper_motor_cmd_state = S_IDLE; // 清除状态
                              LOG_I("stepperMotor_%d_Cur_Speed: %d\n",stepper_motor->stepper_motor_id ,stepper_motor->stepper_motor_speed);
                              rt_mutex_release(mutex_step);
                              LOG_D("mutex_release\n");
                            }
                        } 

                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size));
                          break;
                        }
                        break;
          case S_CPOS : 
                        if(rx_size >= 8)
                        {
                            Emm_V5_Receive(rxCmd, 8);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                            if(rxCmd[1] == 0x36)
                            {
                              //获取绝对值信息
                              float pos = (uint32_t)(
                                                ((uint32_t)rxCmd[3] << 24)    |
                                                ((uint32_t)rxCmd[4] << 16)    |
                                                ((uint32_t)rxCmd[5] << 8)     |
                                                ((uint32_t)rxCmd[6] << 0)
                                              );
                              // 转换为角度
                              stepper_motor->stepper_motor_angle = (float)pos * 360.0f / 65536.0f;

                              // 修正电机正反位置
                              if(rxCmd[2])
                              { stepper_motor->stepper_motor_angle = -stepper_motor->stepper_motor_angle; }
                              LOG_I("stepperMotor_%d_Cur_Pos: %f\n",stepper_motor->stepper_motor_id , stepper_motor->stepper_motor_angle);    
                              rt_mutex_release(mutex_step);
                              LOG_D("mutex_release\n");                          
                            }
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size));
                          break;
                        }
                        break;
          case S_PERR :
                        if(rx_size == 8)
                        {
                            Emm_V5_Receive(rxCmd, 8);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                          //获取绝对值信息
                          float err_pos = (uint32_t)(
                                            ((uint32_t)rxCmd[3] << 24)    |
                                            ((uint32_t)rxCmd[4] << 16)    |
                                            ((uint32_t)rxCmd[5] << 8)     |
                                            ((uint32_t)rxCmd[6] << 0)
                                          );
                          // 转换为角度
                          stepper_motor->stepper_motor_err = (float)err_pos * 360.0f / 65536.0f;

                          // 修正电机正反位置
                          if(rxCmd[2])
                          { stepper_motor->stepper_motor_err = -stepper_motor->stepper_motor_err; }
                          LOG_I("stepperMotor_%d_Cur_err_Pos: %f\n", stepper_motor->stepper_motor_id ,stepper_motor->stepper_motor_err);
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size));
                          break;
                        }
                        break;
          case S_FLAG :
                        if(rx_size == 4)
                        {
                            Emm_V5_Receive(rxCmd, 4);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                          //获取电机状态标志字节
                          uint8_t flag = rxCmd[2];
                          stepper_motor->stepper_motor_reachflag = flag&0x02;
                          stepper_motor->stepper_motor_stallflag = flag&0x04;
                          stepper_motor->stepper_motor_enflag = flag&0x01;
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size));
                          break;
                        }
                        break;
          case S_ORG  : 
                        if(rx_size == 4)
                        {
                            Emm_V5_Receive(rxCmd, 4);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                          //获取电机回零状态标志字节
                          uint8_t flag = rxCmd[2];
                          stepper_motor->stepper_motor_calibrationflag = flag&0x02;
                          stepper_motor->stepper_motor_returnzeroingflag = flag&0x04;
                          stepper_motor->stepper_motor_encokflag = flag&0x01; 
                          stepper_motor->stepper_motor_returnzero_failflag = flag&0x08;  
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size));
                          break;
                        }
                        break;
          case S_Conf :  break;
          case S_State:  break;
          default: LOG_E("stepperMotor_cmd_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, rx_size)); break;
        }
      }
  }
}

void Emm_V5_Transmit(uint8_t* data, uint8_t len)
{
  rt_device_write(Emm_serial1, 0, data, len);
}

int Emm_V5_Receive(uint8_t* data, uint8_t len)
{
  if(rt_device_read(Emm_serial1, 0, data, len) == len)
  {
    return 1;
  }
  else
  {
    LOG_E("stepmotor_receive error!\r\n");
    return 0;
  }
}


rt_device_t Emm_V5_Init(const char* uart)
{
  /* 查找系统中的串口设备 */
  Emm_serial1 = rt_device_find(uart);
  /* 初始化信号量 */
  rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
  /* 以阻塞接收及轮询发送模式打开串口设备 */
  rt_device_open(Emm_serial1, RT_DEVICE_FLAG_RX_NON_BLOCKING | RT_DEVICE_FLAG_TX_BLOCKING);

	return Emm_serial1;
}

void stepper_motor_Init(stepper_motor_t* stepper_motor, uint8_t id)
{
  stepper_motor->stepper_motor_id = id;
}


int stepper_motor_init(void)
{
  /*互斥锁*/
    mutex_step = rt_mutex_create("mutex_step", RT_IPC_FLAG_PRIO);
    if (mutex_step == RT_NULL)
        return -RT_ERROR;

    /* 创建一个动态信号量，初始值是 0 */
    step_sem = rt_sem_create("stem", 0, RT_IPC_FLAG_PRIO);
    if (step_sem == RT_NULL)
    {
        rt_kprintf("create step semaphore failed.\n");
        return -1;
    }
    else
    {
        rt_kprintf("create done. step semaphore value = 0.\n");
    }

    // 创建邮箱mailbox
   rt_err_t result1, result2;

    result1 = rt_mb_init(&mb1,
                         "mbt1",                      /* 名称是 mbt */
                         &mb_pool1[0],                /* 邮箱用到的内存池是 mb_pool */
                         sizeof(mb_pool1) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                         RT_IPC_FLAG_FIFO);           /* 采用 FIFO 方式进行线程等待 */

    if (result1 != RT_EOK)
    {
        rt_kprintf("init mailbox1 failed.\r\n");
        return -1;
    }

  // 创建电机信息处理线程
#define THREAD_PRIORITY_PROCESS_STEPPER_MOTOR    6
#define THREAD_STACK_SIZE_PROCESS_STEPPER_MOTOR  1024
#define THREAD_TIMESLICE_PROCESS_STEPPER_MOTOR    5
		 rt_thread_t process_stepper_motor = RT_NULL;
  
  process_stepper_motor = rt_thread_create("drv_process_steppermotor",
              drv_process_steppermotor, RT_NULL,
              THREAD_STACK_SIZE_PROCESS_STEPPER_MOTOR,
              THREAD_PRIORITY_PROCESS_STEPPER_MOTOR, THREAD_TIMESLICE_PROCESS_STEPPER_MOTOR);

  if (process_stepper_motor != RT_NULL)
  {
    rt_thread_startup(process_stepper_motor);
  }

    /* 创建线程电机执行 */
#define THREAD_PRIORITY_STEPPER_MOTOR    25
#define THREAD_STACK_SIZE_STEPPER_MOTOR  1024
#define THREAD_TIMESLICE_STEPPER_MOTOR    5
		 rt_thread_t tid_stepper_motor = RT_NULL;
  
  tid_stepper_motor = rt_thread_create("drv_stepper_motor",
              drv_stepper_motor, RT_NULL,
              THREAD_STACK_SIZE_STEPPER_MOTOR,
              THREAD_PRIORITY_STEPPER_MOTOR, THREAD_TIMESLICE_STEPPER_MOTOR);
              
  if (tid_stepper_motor != RT_NULL)
  {
    rt_thread_startup(tid_stepper_motor);
  }

	return 0;
}INIT_DEVICE_EXPORT(stepper_motor_init);



uint8_t Emm_V5_ID_judge(stepper_motor_t** stepper_motor)
{
  if(rxCmd[0] == 1)
  {
    *stepper_motor = &stepper_motor_big_arm;
    return  1;
  }
  else if(rxCmd[0] == 3)
  {
    *stepper_motor = &stepper_motor_small_arm;
    return  1;
  }
  else{
    LOG_E("stepperMotor_ID_Error\n");
    return 0;
  }
  return 0;
}