/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-04-26 09:49:09
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-04-26 15:25:50
 * @FilePath: \project\applications\Trajectory_planning.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __TRAJECTORY_PLANNING_H__
#define __TRAJECTORY_PLANNING_H__

#include <rtthread.h>
#include <math.h>

typedef enum PosCurve {
    CURVE_NONE = 0,  //直启
    CURVE_TRAP = 1,  //梯形曲线
    CURVE_SPTA = 2  //S型曲线
}PosCurveType;

/* 定义电机速度曲线对象 */
typedef struct CurveObject {
	float startPos;    //开始调速时的初始位置
	float currentPos;   //当前位置
	float targetPos;   //目标位置度数 单位：度数  
	// float targetPosm;   //目标位置 单位：m  
	float stepPos;    
	float PosMax;     //最大位置
	float PosMin;     //最小位置
	int aTimes;    //调速时间
	int maxTimes;   //调速跨度
	PosCurveType curveMode;  //曲线类型
	float flexible;     //S曲线拉伸度
	float max_pos; //最大位置限制
	int intervel;//调用间隔 单位ms
}CurveObjectType;


static void CalCurveSPTA(CurveObjectType* spta);
void MotorVelocityCurve(CurveObjectType* curve);
void mine_plan(CurveObjectType* curve);

#endif

