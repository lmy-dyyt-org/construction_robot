/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-02 22:00:25
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-09 23:38:54
 * @FilePath: \project\applications\Emm_V5.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __EMM_V5_H
#define __EMM_V5_H
#ifdef __cplusplus
 extern "C" {
#endif

#include <board.h>
#include <rtthread.h>
#include <drv_usart.h>
#include <rtdevice.h>
#include "drv_stepper_motor.h"
#include "ulog.h"
typedef struct stepper_motor stepper_motor_t;

/**********************************************************
***	Emm_V5.0步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

#define		ABS(x)		((x) > 0 ? (x) : -(x)) 

typedef enum {
	S_VER   = 0,			/* 读取固件版本和对应的硬件版本 */
	S_RL    = 1,			/* 读取读取相电阻和相电感 */
	S_PID   = 2,			/* 读取PID参数 */
	S_VBUS  = 3,			/* 读取总线电压 */
	S_CPHA  = 5,			/* 读取相电流 */
	S_ENCL  = 7,			/* 读取经过线性化校准后的编码器值 */
	S_TPOS  = 8,			/* 读取电机目标位置角度 */

	S_VEL   = 9,			/* 读取电机实时转速 */
	S_CPOS  = 10,			/* 读取电机实时位置角度 */      //01 36 01(负位置) 00 01 00 00 6b
	S_PERR  = 11,			/* 读取电机位置误差角度 */
	S_FLAG  = 13,			/* 读取使能/到位/堵转状态标志位 */

	S_Conf  = 14,			/* 读取驱动参数 */
	S_State = 15,			/* 读取系统状态参数 */
	
	S_ORG   = 16,     /* 读取正在回零/回零失败状态标志位 */
	S_IDLE = 17,     /* 读取电机空闲状态标志位 */
	S_Ans = 18,     /* 读取应答命令 */
}SysParams_t;

extern uint8_t rxCmd[128];
extern uint8_t rxCount;
extern int Emm_rx_flag; 
/**********************************************************
*** 注意：每个函数的参数的具体说明，请查阅对应函数的注释说明
**********************************************************/
//如果在发送函数之后缺失了读取回复信息，就会出问题。电机发过来的信息，是直接会被底层存到缓冲区的，如果应用层不去读，底层缓冲区仍然是保留着的。如果后面再读8字节的话，就会把这4个字节先读进去。
//发送就是要死等，你就是发完了，电机才能有下一步动作，你要是不阻塞，怎么知道电机有没有接收到呢。。。。而且这是相对于逻辑层的，逻辑层实际上都是很慢的，以s为单位

void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr); // 将当前位置清零
void Emm_V5_Reset_Clog_Pro(uint8_t addr); // 解除堵转保护
void Emm_V5_Read_Sys_Params(stepper_motor_t* stepper_motor, uint8_t addr, SysParams_t s); // 读取参数
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, rt_bool_t svF, uint8_t ctrl_mode); // 发送命令修改开环/闭环控制模式
void Emm_V5_En_Control(uint8_t addr, rt_bool_t state, rt_bool_t snF); // 电机使能控制
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, rt_bool_t snF); // 速度模式控制
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, rt_bool_t raF, rt_bool_t snF); // 位置模式控制
void Emm_V5_Stop_Now(uint8_t addr, rt_bool_t snF); //让电机立即停止运动
void Emm_V5_Synchronous_motion(uint8_t addr); // 触发多机同步开始运动
void Emm_V5_Origin_Set_O(uint8_t addr, rt_bool_t svF); // 设置挡圈回零的零点位置
void Emm_V5_Origin_Modify_Params(uint8_t addr, rt_bool_t svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, rt_bool_t potF); // 修改回零参数
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, rt_bool_t snF); // 发送命令触发回零
void Emm_V5_Origin_Interrupt(uint8_t addr); // 强制中断并退出回零

void Emm_V5_Get(stepper_motor_t* stepper_motor, SysParams_t s);
#ifdef __cplusplus
}
#endif
#endif
