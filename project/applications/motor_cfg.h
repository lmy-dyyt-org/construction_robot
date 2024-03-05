/**
 * @file motor_cfg.h
 * @author Dyyt587 (67887002+Dyyt587@users.noreply.github.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _MOTOR_CFG_H
#define _MOTOR_CFG_H
#ifdef __cplusplus
extern "C" {
#endif

#define MOTOR_INIT_OPS(index,__ops,__level) [index]={.ops=&((motor_ops_t)__ops),.flag_accept_level=__level}


// #define MOTOR_DJ_M3508_ID1_CAN1
// #define MOTOR_DJ_M3508_ID2_CAN1
// #define MOTOR_DJ_M3508_ID3_CAN1
// #define MOTOR_DJ_M3508_ID4_CAN1
// #define MOTOR_DJ_M3508_ID5_CAN1
// #define MOTOR_DJ_M3508_ID6_CAN1
// #define MOTOR_DJ_M3508_ID7_CAN1
// #define MOTOR_DJ_M3508_ID8_CAN1
// #define MOTOR_DJ_M3508_ID1_CAN2
// #define MOTOR_DJ_M3508_ID2_CAN2
// #define MOTOR_DJ_M3508_ID3_CAN2
// #define MOTOR_DJ_M3508_ID4_CAN2
// #define MOTOR_DJ_M3508_ID5_CAN2
// #define MOTOR_DJ_M3508_ID6_CAN2
// #define MOTOR_DJ_M3508_ID7_CAN2
// #define MOTOR_DJ_M3508_ID8_CAN2

#define MOTOR_DJ_M2006_ID1_CAN1
#define MOTOR_DJ_M2006_ID2_CAN1
#define MOTOR_DJ_M2006_ID3_CAN1
#define MOTOR_DJ_M2006_ID4_CAN1
#define MOTOR_DJ_M2006_ID5_CAN1
#define MOTOR_DJ_M2006_ID6_CAN1
#define MOTOR_DJ_M2006_ID7_CAN1
#define MOTOR_DJ_M2006_ID8_CAN1
#define MOTOR_DJ_M2006_ID1_CAN2
#define MOTOR_DJ_M2006_ID2_CAN2
#define MOTOR_DJ_M2006_ID3_CAN2
#define MOTOR_DJ_M2006_ID4_CAN2
#define MOTOR_DJ_M2006_ID5_CAN2
#define MOTOR_DJ_M2006_ID6_CAN2
#define MOTOR_DJ_M2006_ID7_CAN2
#define MOTOR_DJ_M2006_ID8_CAN2



enum{
#ifdef MOTOR_DJ_M3508_ID1_CAN1
    M3508_1_CAN1,    
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN1
    M3508_2_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN1
    M3508_3_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN1
    M3508_4_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN1
    M3508_5_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID6_CAN1
    M3508_6_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN1
    M3508_7_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN1
    M3508_8_CAN1,
#endif
#ifdef MOTOR_DJ_M3508_ID1_CAN2
    M3508_1_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN2
    M3508_2_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN2
    M3508_3_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN2
    M3508_4_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN2
    M3508_5_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID6_CAN2
    M3508_6_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN2
    M3508_7_CAN2,
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN2
    M3508_8_CAN2,
#endif

#ifdef MOTOR_DJ_M2006_ID1_CAN1
    M2006_1_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID2_CAN1
    M2006_2_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID3_CAN1
    M2006_3_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID4_CAN1
    M2006_4_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID5_CAN1
    M2006_5_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID6_CAN1
    M2006_6_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID7_CAN1
    M2006_7_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID8_CAN1
    M2006_8_CAN1,
#endif
#ifdef MOTOR_DJ_M2006_ID1_CAN2
    M2006_1_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID2_CAN2
    M2006_2_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID3_CAN2
    M2006_3_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID4_CAN2
    M2006_4_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID5_CAN2
    M2006_5_CAN2,
#endif

#ifdef MOTOR_DJ_M2006_ID6_CAN2
    M2006_6_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID7_CAN2
    M2006_7_CAN2,
#endif
#ifdef MOTOR_DJ_M2006_ID8_CAN2
    M2006_8_CAN2,
#endif  
    MOTOR_NUM
};
#ifdef MOTOR_DJ_M3508_ID1_CAN1
#define MOTOR_DJ_M3508_ID1_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_1],                 \
}                                   
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN1
#define MOTOR_DJ_M3508_ID2_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_2],                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID3_CAN1
#define MOTOR_DJ_M3508_ID3_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_3],                \
}
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN1

#define MOTOR_DJ_M3508_ID4_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_4],                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN1
#define MOTOR_DJ_M3508_ID5_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_5],\
}
#endif

#ifdef MOTOR_DJ_M3508_ID6_CAN1
#define MOTOR_DJ_M3508_ID6_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_6],\
}

#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN1
#define MOTOR_DJ_M3508_ID7_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_7],              \
}
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN1
#define MOTOR_DJ_M3508_ID8_CAN1_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN1_8],           \
}
#endif
#ifdef MOTOR_DJ_M3508_ID1_CAN2
#define MOTOR_DJ_M3508_ID1_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_1],                \
}
#endif
#ifdef MOTOR_DJ_M3508_ID2_CAN2
#define MOTOR_DJ_M3508_ID2_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_2],                \
}
#endif

#ifdef MOTOR_DJ_M3508_ID3_CAN2
#define MOTOR_DJ_M3508_ID3_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_3],               \
}
#endif
#ifdef MOTOR_DJ_M3508_ID4_CAN2
#define MOTOR_DJ_M3508_ID4_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_4],                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID5_CAN2
#define MOTOR_DJ_M3508_ID5_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_5],                \
}
#endif
#ifdef MOTOR_DJ_M3508_ID6_CAN2
#define MOTOR_DJ_M3508_ID6_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_6],                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID7_CAN2
#define MOTOR_DJ_M3508_ID7_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_7],                 \
}
#endif
#ifdef MOTOR_DJ_M3508_ID8_CAN2
#define MOTOR_DJ_M3508_ID8_CAN2_OPS \
{                                   \
   .driver= motor_dj_driver,                       \
   .control = motor_dj_ctr,                     \
   .user_data = &dj_motors[DJ_M_CAN2_8],                 \
}
#endif


#ifdef __cplusplus
}
#endif
#endif
