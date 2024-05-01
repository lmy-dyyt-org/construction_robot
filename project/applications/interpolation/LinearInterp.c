#include "linear_Interp.h"
#include <stdio.h>
#include <stdlib.h>
//#include <malloc.h>

int Interpolation_Init(Interpolation_handle_t* interp, void* Interpolation_driver_handle,  int (*Cre)(void* Interpolation_driver_handle), float (*Cal)(void* Interpolation_driver_handle, float x), Point* original_points, int size)
{
    interp->Interpolation_driver_handle = Interpolation_driver_handle;
    interp->Cal = Cal;
    for (int i = 0; i < size; ++i) {
        interp->original_points[i] = original_points[i]; // 赋值原始点
    }
    interp->Cre = Cre;
    interp->size = size;
    if(Cre!=NULL)
    {
        interp->Cre(interp);
    }
    
    return 0;
}

float Interpolate(Interpolation_handle_t* interp, float x)
{
    return interp->Cal(interp, x);
}

// 初始化插值结构体
int Linear_Interpolation_Init(LinearInterpolation* interp)
{  

    return 0;
}

int Linear_Interpolation_Creat(Interpolation_handle_t* interp)// 初始化拟合曲线 
{
    LinearInterpolation* Linear_Interpolation = (LinearInterpolation*)interp->Interpolation_driver_handle;
    Linear_Interpolation->slope = (interp->original_points[0].y - interp->original_points[1].y) / (interp->original_points[0].x - interp->original_points[1].x); // 计算斜率
    return 0;
}

// 根据x坐标计算y值
float Linear_Interpolate(Interpolation_handle_t* interp, float x)
{
    // 使用斜率和已知点的值计算未知点的值
    LinearInterpolation* Linear_Interpolation = (LinearInterpolation *)interp->Interpolation_driver_handle;
    float y = interp->original_points[0].y + Linear_Interpolation->slope * (x - interp->original_points[0].x);
    interp->Interpolation_Out = y;
    return y;
}

int Quadratic_Interpolation_Init(QuadraticFitInterpolation* interp)
{

    return 0;
}

int Quadratic_Interpolation_Creat(Interpolation_handle_t* interp)// 初始化拟合曲线
{
    if (interp == NULL || interp->size < 3 || interp->Interpolation_driver_handle == NULL) {
        return -1;
    }

    QuadraticFitInterpolation* fitInterpolation = (QuadraticFitInterpolation*)interp->Interpolation_driver_handle;
    float sumX = 0.0, sumY = 0.0, sumX2 = 0.0, sumX3 = 0.0, sumXY = 0.0, sumX2Y = 0.0;
    float sumXSquared = 0.0, sumXYSquared = 0.0;

    for (int i = 0; i < interp->size; ++i) {
        float x = interp->original_points[i].x;
        float y = interp->original_points[i].y;
        sumX += x;
        sumY += y;
        sumX2 += x * x;
        sumX3 += x * x * x;
        sumXY += x * y;
        sumX2Y += x * x * y;
        sumXYSquared += x * x * y * y;
    }

    float meanX = sumX / interp->size;
    float meanY = sumY / interp->size;
    float meanX2 = sumX2 / interp->size;
    float meanX3 = sumX3 / interp->size;

    // 计算二次多项式的系数
    float denom = interp->size * sumX2 - sumX * sumX;
    fitInterpolation->a = (interp->size * sumX2Y - sumX * sumXY) / denom;
    fitInterpolation->b = (interp->size * sumXYSquared - meanX * sumXY) / denom - (2 * meanX * fitInterpolation->a);
    fitInterpolation->c = meanY - fitInterpolation->a * meanX2 - fitInterpolation->b * meanX;

    return 0; // 成功拟合
}


float Quadratic_Interpolate(Interpolation_handle_t* interp, float x)
{
    QuadraticFitInterpolation* QuadraticFit_Interpolation = (QuadraticFitInterpolation *)interp->Interpolation_driver_handle;
    float y = QuadraticFit_Interpolation->a * x * x + QuadraticFit_Interpolation->b * x + QuadraticFit_Interpolation->c;
    interp->Interpolation_Out = y;
    return y;
}

int Lagrange_Interpolation_Init(LagrangeInterpolation* interp)
{

    return 0;
}

// 计算拉格朗日基多项式 L_i(x)
float lagrangeBasePoly(Interpolation_handle_t* interp, int index, float x) {
    float result = 1.0;
    for (int j = 0; j < 7; j++) {
        if (j != index) {
            result *= (x - interp->original_points[j].x) / (interp->original_points[index].x - interp->original_points[j].x);
        }
    }
    return result;
}


// 拉格朗日插值法
float Lagrange_Interpolate(Interpolation_handle_t* interp, float x) {
    float result = 0.0;
    for (int i = 0; i < interp->size; i++) {
        result += interp->original_points[i].y * lagrangeBasePoly(interp, i, x);
    }
    interp->Interpolation_Out = result;
    return result;
}






int Spline_Interpolation_Creat(Interpolation_handle_t* interp)
{
    SplineInterpolation* Spline_Interpolation = (SplineInterpolation *)interp->Interpolation_driver_handle;
    int n = interp->size;
    if (n < 4) {
        printf("Not enough data points for cubic spline interpolation.\n");
       return -1;
    }
    typedef struct {
        float u;
        float h;
        float a;
        float b;
        float c;
    }tmp_t;
//    tmp_t* tmp;
//    if (n < 128) {
//        tmp = (tmp_t*)alloca(n * sizeof(tmp_t));
//    }
//    else {
//        while (1);
//    }

//    if (tmp == 0)return 0;
		
		tmp_t tmp[n * sizeof(tmp_t)];
    // 计算 h 和 u 数组
    for (int i = 1; i < n - 1; i++) {
        tmp[i].h = interp->original_points[i].x - interp->original_points[i - 1].x;
        tmp[i].u = (interp->original_points[i + 1].x - interp->original_points[i].x) / tmp[i].h;
    }

    // 应用边界条件：自然边界条件
    Spline_Interpolation->y2[0] = 0.0f;
    Spline_Interpolation->y2[n - 1] = 0.0f;

    tmp[0].a = 0.0f;
    tmp[0].b = 0.0f;
    for (int k = 1; k < n - 1; k++) {
        tmp[k].a = (tmp[k-1].u - tmp[k].u) / tmp[k-1].h;
        tmp[k].b = 2.0f * (tmp[k - 1].h + tmp[k].h);
        tmp[k].c = (tmp[k].u - tmp[k - 1].u) / tmp[k - 1].h;
    }

    // 解线性方程组
    for (int k = 1; k < n - 1; k++) {
        float s = 0.0;
        for (int j = 0; j < k - 1; j++) {
            s += tmp[j].a * Spline_Interpolation->y2[j];
        }
        Spline_Interpolation->y2[k] = (tmp[k].u - s) / tmp[k].b;
    }

    // 回代计算 y2
    for (int k = n - 2; k >= 0; k--) {
        Spline_Interpolation->y2[k] = (Spline_Interpolation->y2[k + 1] - tmp[k].c) / tmp[k].a;
    }

    // 清理分配的内存

    return 0;
}

float Spline_Interpolate(Interpolation_handle_t* interp, float x)
{
    SplineInterpolation* Spline_Interpolation = (SplineInterpolation *)interp->Interpolation_driver_handle;
    int k;
    for (k = 0; k < interp->size - 1; k++) {
        if (x >= interp->original_points[k].x && x <= interp->original_points[k + 1].x) {
            break;
        }
    }

    float h = interp->original_points[k + 1].x - interp->original_points[k].x;
    float a = (Spline_Interpolation->y2[k + 1] - Spline_Interpolation->y2[k]) / (6.0f * h);
    float b = 0.5f * (Spline_Interpolation->y2[k + 1] + Spline_Interpolation->y2[k]);
    float c = (interp->original_points[k + 1].y - interp->original_points[k].y) / h - h * (Spline_Interpolation->y2[k + 1] + 2.0f * Spline_Interpolation->y2[k]) / 6.0f;
    float d = interp->original_points[k].y;

    interp->Interpolation_Out = a * (x - interp->original_points[k].x) * (x - interp->original_points[k + 1].x) * (x - interp->original_points[k + 1].x) + b * (x - interp->original_points[k].x) * (x - interp->original_points[k + 1].x) + c * (x - interp->original_points[k].x) + d;
    return interp->Interpolation_Out;
}
