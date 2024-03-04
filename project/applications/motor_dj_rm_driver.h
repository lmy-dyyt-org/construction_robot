/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-03 17:44:36
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-04 13:15:54
 * @FilePath: \project\applications\motor_dj_rm_driver.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#ifndef MOTOR_DJ_RM_DRIVER_H
#define MOTOR_DJ_RM_DRIVER_H
#if defined(__cplusplus)
extern "C" {
#endif
#include "apid.h"
#include <stdint.h>



#define FILTER_BUF_LEN        5
typedef struct {
    uint16_t id;           //编码器值
    int16_t speed_rpm;      //转速
    float real_current;     //实际输出转矩
    int16_t given_current;  //扭矩
    uint8_t temperature;    //温度
    uint16_t angle;         //abs angle range:[0,8191] 角度范围[0,8191]

    uint16_t last_angle;    //abs angle range:[0,8191]
    uint16_t offset_angle;  //偏差
    int32_t round_cnt;      //电机转的圈数
    int32_t total_angle;    //当前总角度 圈数
    // uint8_t buf_idx;
    // uint16_t angle_buf[FILTER_BUF_LEN];
    // uint16_t fited_angle;        //修正角度？
    uint32_t msg_cnt;            //初始化计数 小于50清零
    float angle_pos;  //绝对值角度
} motor_measure_t;

/*CAN发送或是接收的ID*/
typedef enum {

    CAN_Motor_ALL_ID = 0x200,
    CAN_Motor1_ID = 0x201,
    CAN_Motor2_ID = 0x202,
    CAN_Motor3_ID = 0x203,
    CAN_Motor4_ID = 0x204,

    CAN_Motor_ALL_ID2 = 0x1FF,
    CAN_Motor5_ID = 0x205,
    CAN_Motor6_ID = 0x206,
    CAN_Motor7_ID = 0x207,
    CAN_Motor8_ID = 0x208,

    CAN_6020_ALL_ID = 0X1FF,
    CAN_6020_ID1 = 0x205,
    CAN_6020_ID2 = 0x206,
    CAN_6020_ID3 = 0x207,
    CAN_6020_ID4 = 0x208,
    //6020的id 1234和 2006&3508的id5678重复
    CAN_6020_ALL_ID2 = 0X2FF,
    CAN_6020_ID5 = 0x209,
    CAN_6020_ID6 = 0x20A,
    CAN_6020_ID7 = 0x20B,
    CAN_6020_ID8 = 0x20C,



} DJ_Motor_Message_ID;

int motor_ctr(int id,MOTOR_VALUE_TYPE mode,float*data);
int motor_driver (int id, MOTOR_VALUE_TYPE mode,void* value, void* user_data);

#if defined(__cplusplus)
}
#endif
#endif