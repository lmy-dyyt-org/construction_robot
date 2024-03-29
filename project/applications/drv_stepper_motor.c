/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-06 16:16:14
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-22 19:41:23
 * @FilePath: \project\applications\drv_stepper_motor.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <rtdbg.h>
#define DBG_TAG              "drv.step_motor"
#define DBG_LVL               DBG_INFO
#include "rtthread.h"
#include "drv_stepper_motor.h"
#include "math.h"
#include "robotManager.h"


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
rt_sem_t cmp_sem = RT_NULL;//信号量
uint8_t rxCount = 0;

int32_t big_arm_pulse;
int32_t small_arm_pulse;

int8_t big_arm_dir;
int8_t small_arm_dir;

extern int32_t abig_arm_pulse;
extern int32_t asmall_arm_pulse;
extern float ymm;
extern float zmm;

int8_t big_arm_init_flag;
int8_t small_arm_init_flag;

void drv_stepper_motor(void *parameter)
{
	/*通信初始化 电机初始化*/
  Emm_V5_Init("uart8");
  stepper_motor_Init(&stepper_motor_big_arm, 1);
	stepper_motor_Init(&stepper_motor_small_arm, 3);
  rt_thread_mdelay(2000); //上电需要等两秒 初始化

  //回零调参 电机有存储的 没必要每次都重新设置
  // Emm_V5_Origin_Modify_Params(1, 1, 2, 1, 30, 5000, 300, 700, 60, 0);//参数4是方向   倒数第三个参数是电流值   这是控制大臂的电机（螺丝很长的一边）  方向1 是控制往上抬
  // Emm_V5_Origin_Modify_Params(3, 1, 2, 0, 30, 5000, 300, 700, 60, 0);//参数4是方向 倒数第三个参数是电流值    这是控制小臂的电机（多一个件的一边）  第四个参数 方向0 是控制往上抬

  // rt_thread_mdelay(100); //设置参数之后需要延时！！！！！！！！！延时等待闭环步进参数设置完成（写入flash）
  // Emm_V5_Origin_Trigger_Return(1, 2, 0);
  // Emm_V5_Origin_Trigger_Return(3, 2, 0);

////////////////////上电先到初始位置///////////////////////////////////
 //电机卸力
  Emm_V5_En_Control(1, 0, 0);
  Emm_V5_En_Control(3, 0, 0); 
  while(1)
  {
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_CPOS);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_CPOS);
    //LOG_D("big:%f small:%f",stepper_motor_big_arm.stepper_motor_angle, stepper_motor_small_arm.stepper_motor_angle);
    if( fabs(stepper_motor_big_arm.stepper_motor_angle - 252.7f ) < 1.0f && big_arm_init_flag != 1) 
      {
        LOG_D("big_arm_go_over");
        Emm_V5_En_Control(1, 1, 0);
        big_arm_init_flag = 1;
      }
    if( fabs(stepper_motor_small_arm.stepper_motor_angle - (-45.2f)) < 1.0f && small_arm_init_flag != 1)
      {
        LOG_D("small_arm_go_over");
        Emm_V5_En_Control(3, 1, 0);
        small_arm_init_flag = 1;
      }
    if(big_arm_init_flag == 1 && small_arm_init_flag == 1)
    {
      Emm_V5_Reset_CurPos_To_Zero(1); // 将当前位置清零
      Emm_V5_Reset_CurPos_To_Zero(3); // 将当前位置清零
      break;
    }
    rt_thread_mdelay(10);
  }
  extern uint8_t rbmg_mode;
  rbmg_mode = LINE_MODE;
////////////////////////////////////////////////////////////////////////


  while(1)
  {
		// Emm_V5_Vel_Control(1, 0, 100, 0, 0); 
    // rt_thread_mdelay(600);//这里的延时要根据 速度 和 转动圈数来取

    big_arm_pulse = abig_arm_pulse;
    small_arm_pulse = asmall_arm_pulse;

    if(big_arm_pulse>0)
    {
      big_arm_dir = 0;
    }
    else
    {
      big_arm_pulse = -big_arm_pulse;
      big_arm_dir = 1;
    }

    if(small_arm_pulse>0)
    {
      small_arm_dir = 1;
    }
    else
    {
      small_arm_pulse = -small_arm_pulse;
      small_arm_dir = 0;
    }

    // LOG_D("big:%d small:%d",big_arm_pulse, small_arm_pulse);  //int32 用 %f 打印会有问题
    Emm_V5_Pos_Control(1, big_arm_dir, 100, 20, big_arm_pulse, 1, 0);
    Emm_V5_Pos_Control(3, small_arm_dir, 100, 20, small_arm_pulse, 1, 0);

    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_CPOS);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_VEL);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_PERR);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_FLAG);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_ORG);
    Emm_V5_Read_Sys_Params(&stepper_motor_big_arm, 1, S_CPHA);

    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_CPOS);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_VEL);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_PERR);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_FLAG);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_ORG);
    Emm_V5_Read_Sys_Params(&stepper_motor_small_arm, 3, S_CPHA);

    LOG_D("big_current:%d small_current:%d",stepper_motor_big_arm.stepper_motor_current, stepper_motor_small_arm.stepper_motor_current);
    rt_thread_mdelay(500);
  }
}

/* 接收数据回调函数 */
static rt_err_t Emm_uart_receive_callback(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    // rt_sem_release(&rx_sem);

    /* 发送 缓冲区大小 到邮箱中 */
    rxCount = size;
    switch(stepper_motor_cmd_state)                           // 功能码
    {
      case S_VER  :  break;
      case S_RL   :  break;
      case S_PID  :  break;
      case S_VBUS :  break;
      case S_CPHA : 
        if(size>=5)  
        {
          // LOG_D("vel,size:%d\n",size);
      // rt_mb_send(&mb1, (uint8_t)size); 
          rt_sem_release(cmp_sem);
        }
        break;
      case S_ENCL :  break;
      case S_TPOS :  break;
      case S_VEL  :  
        if(size>=6)  
        {
          // LOG_D("vel,size:%d\n",size);
      // rt_mb_send(&mb1, (uint8_t)size); 
          rt_sem_release(cmp_sem);
        }
      break;
      case S_CPOS :  
        if(size>=8)  
        {
          // LOG_D("pos,size:%d\n",size);
          // rt_mb_send(&mb1, (uint8_t)size);
          rt_sem_release(cmp_sem);
        }
        break;
      case S_PERR :  
        if(size>=8)  
        {
       // rt_mb_send(&mb1, (uint8_t)size);
          rt_sem_release(cmp_sem);
        }
        
        break;
      case S_FLAG :  
        if(size>=4)  
        {
          // rt_mb_send(&mb1, (uint8_t)size); 
          rt_sem_release(cmp_sem);
        }
        break;
      case S_ORG  :  
        if(size>=4)
        {
          // rt_mb_send(&mb1, (uint8_t)size);
          rt_sem_release(cmp_sem);
        }
        break;
      case S_Conf :  break;
      case S_State:  break;
      case S_IDLE :  
        // LOG_D("idle,size:%d\n",size);  
        // rt_mb_send(&mb1, (uint8_t)size);
        rt_sem_release(cmp_sem);
        break;
      case S_Ans :  
        if(size>=4) 
        {
          // LOG_D("ansewer,size:%d\n",size); 
        // rt_mb_send(&mb1, (uint8_t)size); 
          rt_sem_release(cmp_sem);
        }
        break;
      default: while(1){
        LOG_E("stepperMotor_cmd_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, rxCmd, 60));
      }break;
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
  // LOG_D("stepperMotor_Receive_Error: %d\n", t);

  /* 设置接收回调函数 */
  rt_device_set_rx_indicate(Emm_serial1, Emm_uart_receive_callback);  

  while(1)
  {
        rt_sem_take(step_sem, RT_WAITING_FOREVER); //获取通知
        // LOG_D("rece_take_sem\n");
        rt_mutex_take(mutex_step, RT_WAITING_FOREVER);  //获取电机处理权
        // LOG_D("rece_take_mutex\n");
      /* 从邮箱中收取邮件 */
      // if (rt_mb_recv(&mb1, &rx_size, RT_WAITING_FOREVER) == RT_EOK)
      // {
        rt_sem_take(cmp_sem, RT_WAITING_FOREVER); //获取callback通知
        // LOG_D("start_process,rx_size:%d\n",rxCount);
        // rt_device_read(Emm_serial1, 0, &data, 1);
        // LOG_I("stepperMotor_Receive: %d,%x,%d\n",rx_size,data,++rx_count);
        switch(stepper_motor_cmd_state)                           // 功能码
        {
          case S_IDLE : 
                        LOG_D("stepperMotor_Receive: %x,%d,%d\n",data[0],rt_device_read(Emm_serial1, 0, data, 60),++rx_count);//把缓冲区的垃圾清空
                         break;
          case S_VER  :  break;
          case S_RL   :  break;
          case S_PID  :  break;
          case S_VBUS :  break;
          case S_CPHA :  
                       if(rxCount >= 5)
                        {
                            Emm_V5_Receive(rxCmd, 5);
                            if(Emm_V5_ID_judge(&stepper_motor) == 0)
                            {
                              break;
                            }
                            if(rxCmd[1] == 0x27)
                            {  
                              stepper_motor->stepper_motor_current = (uint16_t)(
                                                ((uint16_t)rxCmd[2] << 8)    |
                                                ((uint16_t)rxCmd[3] << 0)    
                                              );
                              stepper_motor_cmd_state = S_IDLE; // 清除状态
                              LOG_I("stepperMotor_%d_Cur_Speed: %d\n",stepper_motor->stepper_motor_id ,stepper_motor->stepper_motor_current);
                              rt_mutex_release(mutex_step);
                              // LOG_D("mutex_release\n");
                            }
                        } 

                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_ENCL :  break;
          case S_TPOS :  break;
          case S_VEL  :  
                        if(rxCount >= 6)
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
                              // LOG_D("mutex_release\n");
                            }
                        } 

                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_CPOS : 
                        if(rxCount >= 8)
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
                               stepper_motor_cmd_state = S_IDLE; // 清除状态
                              LOG_I("stepperMotor_%d_Cur_Pos: %f\n",stepper_motor->stepper_motor_id , stepper_motor->stepper_motor_angle);    
                              rt_mutex_release(mutex_step);
                              // LOG_D("mutex_release\n");                          
                            }
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_PERR :
                        if(rxCount >= 8)
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
                          stepper_motor_cmd_state = S_IDLE; // 清除状态
                          LOG_I("stepperMotor_%d_Cur_err_Pos: %f\n", stepper_motor->stepper_motor_id ,stepper_motor->stepper_motor_err);
                          rt_mutex_release(mutex_step);
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_FLAG :
                        if(rxCount >= 4)
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
                          stepper_motor_cmd_state = S_IDLE; // 清除状态
                          rt_mutex_release(mutex_step);
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_ORG  : 
                        if(rxCount >= 4)
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
                          stepper_motor_cmd_state = S_IDLE; // 清除状态
                          rt_mutex_release(mutex_step);
                        } 
                        else 
                        {
                          LOG_E("stepperMotor_Vel_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60));
                          break;
                        }
                        break;
          case S_Conf :  break;
          case S_State:  break;
          case S_Ans :
          rt_device_read(Emm_serial1, 0, tmp, 60);
            // LOG_D("process_answer: %d\n", ); 
            stepper_motor_cmd_state = S_IDLE;  rt_mutex_release(mutex_step); 
            break;
          default: LOG_E("stepperMotor_cmd_Receive_Error: %d\n", rt_device_read(Emm_serial1, 0, tmp, 60)); rt_mutex_release(mutex_step); break;
        }
      // }
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

    /* 创建一个动态信号量1，初始值是 0 */
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

    /* 创建一个动态信号量2，初始值是 0 */
    cmp_sem = rt_sem_create("cpem", 0, RT_IPC_FLAG_PRIO);
    if (cmp_sem == RT_NULL)
    {
        rt_kprintf("create cmp semaphore failed.\n");
        return -1;
    }
    else
    {
        rt_kprintf("create done. cmp semaphore value = 0.\n");
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

void arm_y_add20(void)
{
	ymm += 20;
}
MSH_CMD_EXPORT(arm_y_add20, arm_y_add20);
void arm_y_reduce20(void)
{
	ymm -= 20;
}
MSH_CMD_EXPORT(arm_y_reduce20, arm_y_reduce20);
void arm_z_add20(void)
{
	zmm += 20;
}
MSH_CMD_EXPORT(arm_z_add20, arm_z_add20);
void arm_z_reduce20(void)
{
	zmm -= 20;
}
MSH_CMD_EXPORT(arm_z_reduce20, arm_z_reduce20);
void arm_ctrl(int argc, char **argv)
{
	if(argc != 3)
	{
		rt_kprintf("正确格式：rm_ctrl ymm zmm\n");
		return; 
	}           
  ymm = atof(argv[1]);
  zmm = atof(argv[2]);
  LOG_D("ymm:%f,zmm:%f",ymm,zmm);
}
MSH_CMD_EXPORT(arm_ctrl, "arm_ymm,zmm"); 
void arm_h(void)
{
  LOG_D("arm_y_add20  arm_y_add20\n");  
  LOG_D("arm_z_add20  arm_z_add20\n"); 
  LOG_D("arm_y_reduce20  arm_y_reduce20\n");
  LOG_D("arm_z_reduce20  arm_z_reduce20\n");
  LOG_D("arm_ctrl  arm_ymm,zmm\n"); 
}
MSH_CMD_EXPORT(arm_h, "arm command help");
MSH_CMD_EXPORT_ALIAS(arm_h,arm, "arm command help");


// CMD_OPTIONS_STATEMENT(cmd_list)
// static int cmd_list(int argc, char **argv)
// {
//     if (argc == 2)
//     {
//         switch (MSH_OPT_ID_GET(cmd_list))
//         {
//         case RT_Object_Class_Thread: list_thread(); break;
//         case RT_Object_Class_Timer: list_timer(); break;
// #ifdef RT_USING_SEMAPHORE
//         case RT_Object_Class_Semaphore: list_sem(); break;
// #endif /* RT_USING_SEMAPHORE */
// #ifdef RT_USING_EVENT
//         case RT_Object_Class_Event: list_event(); break;
// #endif /* RT_USING_EVENT */
// #ifdef RT_USING_MUTEX
//         case RT_Object_Class_Mutex: list_mutex(); break;
// #endif /* RT_USING_MUTEX */
// #ifdef RT_USING_MAILBOX
//         case RT_Object_Class_MailBox: list_mailbox(); break;
// #endif  /* RT_USING_MAILBOX */
// #ifdef RT_USING_MESSAGEQUEUE
//         case RT_Object_Class_MessageQueue: list_msgqueue(); break;
// #endif /* RT_USING_MESSAGEQUEUE */
// #ifdef RT_USING_MEMHEAP
//         case RT_Object_Class_MemHeap: list_memheap(); break;
// #endif /* RT_USING_MEMHEAP */
// #ifdef RT_USING_MEMPOOL
//         case RT_Object_Class_MemPool: list_mempool(); break;
// #endif /* RT_USING_MEMPOOL */
// #ifdef RT_USING_DEVICE
//         case RT_Object_Class_Device: list_device(); break;
// #endif /* RT_USING_DEVICE */
// #ifdef RT_USING_DFS
//         case LIST_DFS_OPT_ID:
//         {
//             extern int list_fd(void);
//             list_fd();
//             break;
//         }
// #endif /* RT_USING_DFS */
//         default:
//             goto _usage;
//         };

//         return 0;
//         }

// _usage:
//     rt_kprintf("Usage: list [options]\n");
//     rt_kprintf("[options]:\n");
//     MSH_OPT_DUMP(cmd_list);
//     return 0;
// }
// CMD_OPTIONS_NODE_START(cmd_arm)
// CMD_OPTIONS_NODE(RT_Object_Class_Thread,       thread,   list threads)
// CMD_OPTIONS_NODE(RT_Object_Class_Timer,        timer,    list timers)
// #ifdef RT_USING_SEMAPHORE
// CMD_OPTIONS_NODE(RT_Object_Class_Semaphore,    sem,      list semaphores)
// #endif /* RT_USING_SEMAPHORE */
// #ifdef RT_USING_EVENT
// CMD_OPTIONS_NODE(RT_Object_Class_Event,        event,    list events)
// #endif /* RT_USING_EVENT */
// #ifdef RT_USING_MUTEX
// CMD_OPTIONS_NODE(RT_Object_Class_Mutex,        mutex,    list mutexs)
// #endif /* RT_USING_MUTEX */
// #ifdef RT_USING_MAILBOX
// CMD_OPTIONS_NODE(RT_Object_Class_MailBox,      mailbox,  list mailboxs)
// #endif  /* RT_USING_MAILBOX */
// #ifdef RT_USING_MESSAGEQUEUE
// CMD_OPTIONS_NODE(RT_Object_Class_MessageQueue, msgqueue, list message queues)
// #endif /* RT_USING_MESSAGEQUEUE */
// #ifdef RT_USING_MEMHEAP
// CMD_OPTIONS_NODE(RT_Object_Class_MemHeap,      memheap,  list memory heaps)
// #endif /* RT_USING_MEMHEAP */
// #ifdef RT_USING_MEMPOOL
// CMD_OPTIONS_NODE(RT_Object_Class_MemPool,      mempool,  list memory pools)
// #endif /* RT_USING_MEMPOOL */
// #ifdef RT_USING_DEVICE
// CMD_OPTIONS_NODE(RT_Object_Class_Device,       device,   list devices)
// #endif /* RT_USING_DEVICE */
// #ifdef RT_USING_DFS
// CMD_OPTIONS_NODE(LIST_DFS_OPT_ID,              fd,       list file descriptors)
// #endif /* RT_USING_DFS */
// CMD_OPTIONS_NODE_END
// MSH_CMD_EXPORT_ALIAS(cmd_list, list, list objects, optenable);