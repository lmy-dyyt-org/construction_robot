/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-19 09:10:31
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-01 21:10:12
 * @FilePath: \project\applications\robotManager\robotmanager.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <rtthread.h>
#define DBG_TAG "RobotManager"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "robotManager.h"
#include "abus_topic.h"
#include "chassis_port.h"
#include "abus_topic.h"
#include "math.h"
//#include "PathFinder.h"
#include "aboard_power_switch.h"
#include "linear_Interp.h"
#include "drv_corexy.h"

Point imagecenter;//corexy到绘图坐标系的偏移
Point rightbottom;//corexy坐标系下图像的右下角顶点 (给插值器用的零点)

Interpolation_handle_t square_interpolation_a;
LinearInterpolation square_interpolation_handle_a;
Interpolation_handle_t square_interpolation_b;
LinearInterpolation square_interpolation_handle_b;
Interpolation_handle_t square_interpolation_c;
LinearInterpolation square_interpolation_handle_c;
Interpolation_handle_t square_interpolation_d;
LinearInterpolation square_interpolation_handle_d;

Point now_points = {0,0};

// void corexy2graphics(const Point* corexy,Point* graphics)
// {
//     graphics->x = corexy->x + imagecenter.x;
//     graphics->y = corexy->y + imagecenter.y;
// }

void graphics2corexy(Point* corexy,const Point* graphics)
{
    corexy->x = graphics->x + imagecenter.x;
    corexy->y = graphics->y + imagecenter.y;
}
enum{
    A=0U,
    B, 
    C,
    D,
};

//以知道矩形中心点，矩形右下角顶点坐标 //均在绘图坐标系
int draw_square(uint32_t time_ms)
{

    static float delta = 0;
    delta = sqrt( pow((imagecenter.x-rightbottom.x),2) + pow((imagecenter.y-rightbottom.y),2) );
    static float c = 0; //边长
    c = delta/sqrt(2);

    // static Point points_01[2]={{0,0},{0,c}};
    // static Point points_12[2]={{0,c},{-c,c}};
    // static Point points_23[2]={{-c,c},{-c,0}};
    // static Point points_30[2]={{-c,0},{0,0}};

    static Point points_01[2]={0};
    static Point points_12[2]={0};
    static Point points_23[2]={0};
    static Point points_30[2]={0};

    points_01[1].y=c;
    points_12[0].y=c; points_12[1].x=-c; points_12[1].y=c;
    points_23[0].x=-c; points_23[0].y=c; points_23[1].x=-c;
    points_30[0].x=-c;

    static Point draw_points={0,0};

    if(time_ms == 0)
    {
        Interpolation_Init(&square_interpolation_a, &square_interpolation_handle_a, Linear_Interpolation_Creat,Linear_Interpolate,points_01, 2);
        Interpolation_Init(&square_interpolation_b, &square_interpolation_handle_b, Linear_Interpolation_Creat,Linear_Interpolate,points_12, 2);
        Interpolation_Init(&square_interpolation_c, &square_interpolation_handle_c, Linear_Interpolation_Creat,Linear_Interpolate,points_23, 2);
        Interpolation_Init(&square_interpolation_d, &square_interpolation_handle_d, Linear_Interpolation_Creat,Linear_Interpolate,points_30, 2);
    }
    
    static int step=A;//用于指示哪一段曲线a,b,c,d
    /*绘图坐标系下 以右下角为原点 （0，0）
   2____b____1
    |       |
   c|       |a
   3|___d___|0
    */
   if(draw_points.x==0 && draw_points.y>0 && draw_points.y<c)
    {
        step = A;
    }
    else if (draw_points.y==c)
    {
        step = B;
    }
    else if (draw_points.x==-c && draw_points.y>0 && draw_points.y<c)
    {
        step = C;
    }
    else if(draw_points.y==0)
    {
        step = D;
    }    
    else
    {
        LOG_E("draw error!");
    }

    switch(step)
    {
        case A:
            Linear_Interpolate(&square_interpolation_a,time_ms);
            draw_points.x = time_ms; 
            draw_points.y = square_interpolation_a.Interpolation_Out;
            graphics2corexy(&now_points, &draw_points);
            break;
        case B:
            Linear_Interpolate(&square_interpolation_b,time_ms);
            draw_points.x = time_ms; 
            draw_points.y = square_interpolation_b.Interpolation_Out;
            graphics2corexy(&now_points, &draw_points);
            break;
        case C:
            Linear_Interpolate(&square_interpolation_c,time_ms);
            draw_points.x = time_ms; 
            draw_points.y = square_interpolation_c.Interpolation_Out;
            graphics2corexy(&now_points, &draw_points);
            break;
        case D:
            Linear_Interpolate(&square_interpolation_d,time_ms);
            draw_points.x = time_ms; 
            draw_points.y = square_interpolation_d.Interpolation_Out;
            graphics2corexy(&now_points, &draw_points);
            break;                                    
    }
    
    return 0;
}
void rbmg_handle(void*param)
{

    while(1)
    {
        now_points.x = corexy.x; 
        now_points.y = corexy.y; 


        rt_thread_mdelay(100);
    }
}

int rbmg_init(void)
{
    rt_thread_t tid_rbmg = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_rbmg = rt_thread_create("robotmanger",
                                rbmg_handle, RT_NULL,
                                4096,
                                7, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_rbmg != RT_NULL)
    {
        rt_thread_startup(tid_rbmg);
    }
    return 0;
}
INIT_APP_EXPORT(rbmg_init);