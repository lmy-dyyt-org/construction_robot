#ifndef _LINEAR_INTERP_H_
#define _LINEAR_INTERP_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 定义一个结构体用于存储线性插值所需的已知点
typedef struct {
    float x;
    float y;
} Point;

// 定义一个结构体用于存储线性插值的相关信息
typedef struct {

    float slope; // 斜率

} LinearInterpolation;


// 定义一个结构体用于存储线性插值的相关信息
/*
    
*/
typedef struct {

    Point ori; // 原点
    Point cfg; // x半长轴 y半短轴
} CircleInterpolation_t;



typedef struct {
    float a; // 二次项系数
    float b; // 一次项系数
    float c; // 常数项
} QuadraticFitInterpolation;

typedef struct {
    float a;
} LagrangeInterpolation;

typedef struct {
    float y2[8]; // 二阶导数数组
} SplineInterpolation;

typedef struct Interpolation_handle_t
{
    Point original_points[8];
    int size;

    void* Interpolation_driver_handle;

    // 调用拟合
    int (*Cre)(void* Interpolation_driver_handle);
    // 调用计算
    float (*Cal)(void* Interpolation_driver_handle, float x);

    float Interpolation_Out;

}Interpolation_handle_t;

int Interpolation_Init(Interpolation_handle_t* interp, void* Interpolation_driver_handle, int (*Cre)(void* Interpolation_driver_handle), float (*Cal)(void* Interpolation_driver_handle, float x), Point* original_points, int size);
float Interpolate(Interpolation_handle_t* interp, float x);

int Linear_Interpolation_Init(LinearInterpolation* interp);//先有驱动（底层），再有调用
int Linear_Interpolation_Creat(Interpolation_handle_t* interp);// 初始化拟合曲线 
float Linear_Interpolate(Interpolation_handle_t* interp, float x);

int Quadratic_Interpolation_Init(QuadraticFitInterpolation* interp);
int Quadratic_Interpolation_Creat(Interpolation_handle_t* interp);// 初始化拟合曲线
float Quadratic_Interpolate(Interpolation_handle_t* interp, float x);

int Lagrange_Interpolation_Init(LagrangeInterpolation* interp);int Lagrange_Interpolation_Init(LagrangeInterpolation* interp);
float lagrangeBasePoly(Interpolation_handle_t* interp, int index, float x);
float Lagrange_Interpolate(Interpolation_handle_t* interp, float x);

int Spline_Interpolation_Init(SplineInterpolation* interp); 
int Spline_Interpolation_Creat(Interpolation_handle_t* interp);
float Spline_Interpolate(Interpolation_handle_t* interp, float x);

#ifdef __cplusplus
}
#endif

#endif
#pragma once
