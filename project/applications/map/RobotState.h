#ifndef ROBOTSTATE_H
#define ROBOTSTATE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "SemanticMap.h"
#include "PathFinder.h"
#include "SemanticMap.h"

#ifndef LOG_D
#define LOG_D printf
#endif
#ifndef LOG_E
#define LOG_E printf
#endif
typedef struct{
    int x;
    int y;
}point_t;
typedef struct{
    Path_table_t* table;
    Smt_map_t** map;
    
   point_t now_state;
    point_t will_state;
}Robote_State_t;

//触发机器人请求寻路器并且更新位置且发送信息
void Robot_State_Trigger(Robote_State_t* robot_state);

#ifdef __cplusplus
}
#endif
#endif