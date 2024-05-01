#ifndef _LINEAR_INTERP_H_
#define _LINEAR_INTERP_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ����һ���ṹ�����ڴ洢���Բ�ֵ�������֪��
typedef struct {
    float x;
    float y;
} Point;

// ����һ���ṹ�����ڴ洢���Բ�ֵ�������Ϣ
typedef struct {

    float slope; // б��

} LinearInterpolation;


// ����һ���ṹ�����ڴ洢���Բ�ֵ�������Ϣ
/*
    
*/
typedef struct {

    Point ori; // ԭ��
    Point cfg; // x�볤�� y�����
} CircleInterpolation_t;



typedef struct {
    float a; // ������ϵ��
    float b; // һ����ϵ��
    float c; // ������
} QuadraticFitInterpolation;

typedef struct {
    float a;
} LagrangeInterpolation;

typedef struct {
    float y2[8]; // ���׵�������
} SplineInterpolation;

typedef struct Interpolation_handle_t
{
    Point original_points[8];
    int size;

    void* Interpolation_driver_handle;

    // �������
    int (*Cre)(void* Interpolation_driver_handle);
    // ���ü���
    float (*Cal)(void* Interpolation_driver_handle, float x);

    float Interpolation_Out;

}Interpolation_handle_t;

int Interpolation_Init(Interpolation_handle_t* interp, void* Interpolation_driver_handle, int (*Cre)(void* Interpolation_driver_handle), float (*Cal)(void* Interpolation_driver_handle, float x), Point* original_points, int size);
float Interpolate(Interpolation_handle_t* interp, float x);

int Linear_Interpolation_Init(LinearInterpolation* interp);//�����������ײ㣩�����е���
int Linear_Interpolation_Creat(Interpolation_handle_t* interp);// ��ʼ��������� 
float Linear_Interpolate(Interpolation_handle_t* interp, float x);

int Quadratic_Interpolation_Init(QuadraticFitInterpolation* interp);
int Quadratic_Interpolation_Creat(Interpolation_handle_t* interp);// ��ʼ���������
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
