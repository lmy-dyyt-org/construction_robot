/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-19 09:10:31
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-03 13:50:40
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
// #include "PathFinder.h"
#include "aboard_power_switch.h"
#include "linear_Interp.h"
#include "drv_corexy.h"
#include "drv_emm_v5.h"
#include "apid.h"

#ifndef max
#define max(a, b) (a) > (b) ? (a) : (b)
#endif

Point imagecenter = {0.0, 0.0}; // corexy到绘图坐标系的偏移
Point rightbottom;              // corexy坐标系下图像的右下角顶点 (给插值器用的零点)

Interpolation_handle_t square_interpolation_a;
LinearInterpolation square_interpolation_handle_a;
Interpolation_handle_t square_interpolation_b;
LinearInterpolation square_interpolation_handle_b;
Interpolation_handle_t square_interpolation_c;
LinearInterpolation square_interpolation_handle_c;
Interpolation_handle_t square_interpolation_d;
LinearInterpolation square_interpolation_handle_d;

Point now_points = {0, 0};

apid_t pid_centerx;
float pid_outx;
float now_datax;

apid_t pid_centery;
float pid_outy;
float now_datay;
// void corexy2graphics(const Point* corexy,Point* graphics)
// {
//     graphics->x = corexy->x + imagecenter.x;
//     graphics->y = corexy->y + imagecenter.y;
// }

void graphics2corexy(Point *corexy, const Point *graphics)
{
    corexy->x = graphics->x + imagecenter.x;
    corexy->y = graphics->y + imagecenter.y;
}

enum
{
    A = 0U,
    B,
    C,
    D,
    OVER,
};

int draw_line(Point *start, Point *end)
{
    float deltax = 0.001;
    float draw_x = 0;
    int temp = 0;
    // init
    Interpolation_handle_t interp;
    LinearInterpolation square_interpolation_handle;

    Point data[2] = {*start, *end};
    Interpolation_Init(&interp, &square_interpolation_handle, Linear_Interpolation_Creat, Linear_Interpolate, data, 2);
    draw_x = start->x;
    if (start->x > end->x)
    {
        deltax = -deltax;
        temp = 1;
    }
    while (1)
    {
        draw_x += deltax;
        if (temp)
        {
            if (draw_x < end->x)
            {
                draw_x = end->x;
            }
        }
        else
        {
            if (draw_x > end->x)
            {
                draw_x = end->x;
            }
        }

        Linear_Interpolate(&interp, draw_x);
        Point draw_points = {draw_x, interp.Interpolation_Out};
        graphics2corexy(&now_points, &draw_points);
        corexy_absolute_move(&corexy, now_points.x, now_points.y);
        float offset = max(fabs(now_points.y - corexy.y), fabs(now_points.x - corexy.x));
        gap_time = fabs((offset * 1000 / ((motor_vel * 0.04f) / 60.f))); // 有两个gap_time延时,这里的是为了不要快速的去算插值，导致跳跃很大。 还有一个延时（drv emm v5），是让电机运动到目标位置，再开始下一段插值。
        // rt_thread_mdelay(gap_time);
        //  rt_thread_mdelay(500);
        Emm_V5_Pos_moveok();
        // 添加到达终点的判断条件，这里假设delta始终指向终点方向
        if (fabs(draw_x - end->x) < 0.005f)
        {
            return 0; // 到达终点，退出循环
        }
    }
}

// 以知道矩形中心点，矩形右下角顶点坐标 //均在绘图坐标系
int draw_square(void)
{
#define delta 0.001

    static float draw_x = 0;
    // static float tmp = 0;
    // tmp = sqrt( pow((imagecenter.x-rightbottom.x),2) + pow((imagecenter.y-rightbottom.y),2) );
    static float c = 0; // 边长
    // c = tmp/sqrt(2);
    c = 0.25;
    float offset = 0; // 每次插值移动的距离

    // static Point points_01[2]={{0,0},{0,c}};
    // static Point points_12[2]={{0,c},{-c,c}};
    // static Point points_23[2]={{-c,c},{-c,0}};
    // static Point points_30[2]={{-c,0},{0,0}};

    static Point points_01[2] = {0};
    static Point points_12[2] = {0};
    static Point points_23[2] = {0};
    static Point points_30[2] = {0};

    points_01[1].y = c;
    points_12[0].y = c;
    points_12[1].x = -c;
    points_12[1].y = c;
    points_23[0].x = -c;
    points_23[0].y = c;
    points_23[1].x = -c;
    points_30[0].x = -c;

    static Point draw_points = {0, 0};

    if (draw_x == 0)
    {
        Interpolation_Init(&square_interpolation_a, &square_interpolation_handle_a, Linear_Interpolation_Creat, Linear_Interpolate, points_01, 2);
        Interpolation_Init(&square_interpolation_b, &square_interpolation_handle_b, Linear_Interpolation_Creat, Linear_Interpolate, points_12, 2);
        Interpolation_Init(&square_interpolation_c, &square_interpolation_handle_c, Linear_Interpolation_Creat, Linear_Interpolate, points_23, 2);
        Interpolation_Init(&square_interpolation_d, &square_interpolation_handle_d, Linear_Interpolation_Creat, Linear_Interpolate, points_30, 2);
    }

    static int step = A; // 用于指示哪一段曲线a,b,c,d
    /*绘图坐标系下 以图形右下角为原点 （0，0）
   2____b____1
    |       |
   c|       |a
   3|___d___|0
    */

    switch (step)
    {
    case A:
        draw_line(&points_01[0], &points_01[1]);
        step = B;
        break;
    case B:
        draw_line(&points_12[0], &points_12[1]);
        step = C;
        break;
    case C:
        draw_line(&points_23[0], &points_23[1]);
        step = D;
        break;
    case D:
        draw_line(&points_30[0], &points_30[1]);
        step = OVER;
        break;
    default:
        // LOG_E("step error case");
        break;
    }
    return 0;
}

// 以知道矩形中心点，矩形右下角顶点坐标 //均在绘图坐标系
int draw_triangle(void)
{
#define delta2 0.001
    static int time = 0;
    static float draw_x = 0;
    // static float tmp = 0;
    // tmp = sqrt( pow((imagecenter.x-rightbottom.x),2) + pow((imagecenter.y-rightbottom.y),2) );
    static float c = 0; // 边长
    // c = sqrt(3)*tmp;
    c = 0.3;
    float offset = 0; // 每次插值移动的距离

    // static Point points_01[2]={{0,0},{-1.732c/2,3c/2}};
    // static Point points_12[2]={{-1.732c/2,3c/2},{-c,0}};
    // static Point points_20[2]={{-c,0},{0,0}};

    static Point points_01[2] = {0};
    static Point points_12[2] = {0};
    static Point points_20[2] = {0};

    points_01[1].x = -0.5 * c;
    points_01[1].y = 0.866 * c;
    points_12[0].x = -0.5 * c;
    points_12[0].y = 0.866 * c;
    points_12[1].x = -c;
    points_20[0].x = -c;

    static Point draw_points = {0, 0};

    if (draw_x == 0)
    {
        LOG_D("points_0.x:%f,points_0.y:%f", points_01[0].x, points_01[0].y);
        LOG_D("points_1.x:%f,points_1.y:%f", points_01[1].x, points_01[1].y);
        LOG_D("points_2.x:%f,points_2.y:%f", points_12[1].x, points_12[1].y);
        Interpolation_Init(&square_interpolation_a, &square_interpolation_handle_a, Linear_Interpolation_Creat, Linear_Interpolate, points_01, 2);
        Interpolation_Init(&square_interpolation_b, &square_interpolation_handle_b, Linear_Interpolation_Creat, Linear_Interpolate, points_12, 2);
        Interpolation_Init(&square_interpolation_c, &square_interpolation_handle_c, Linear_Interpolation_Creat, Linear_Interpolate, points_20, 2);
    }

    static int step = A; // 用于指示哪一段曲线a,b,c
    /*绘图坐标系下 以图形右下角为原点 （0，0）
        1
        /\
       /  \
    b /    \a
   2 /______\ 0
        c
    */
    switch (step)
    {
    case A:
        draw_line(&points_01[0], &points_01[1]);
        step = B;
        break;
    case B:
        draw_line(&points_12[0], &points_12[1]);
        step = C;
        break;
    case C:
        draw_line(&points_20[0], &points_20[1]);
        step = OVER;
        break;
    default:
        // LOG_E("step error case");
        break;
    }
    return 0;
}

int draw_triangle_rotate(void)
{
#define delta2 0.001
    static int time = 0;
    static float draw_x = 0;
    // static float tmp = 0;
    // tmp = sqrt( pow((imagecenter.x-rightbottom.x),2) + pow((imagecenter.y-rightbottom.y),2) );
    static float c = 0; // 边长
    // c = sqrt(3)*tmp;
    c = 0.1;
    float offset = 0;       // 每次插值移动的距离
    float angle = 3.14 / 4; /*0--2*pi*/
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);

    static Point points_01[2] = {0};
    static Point points_12[2] = {0};
    static Point points_20[2] = {0};

    // points_01[1].x=-0.5*c; points_01[1].y=0.866*c;
    // points_12[0].x=-0.5*c; points_12[0].y=0.866*c; points_12[1].x=-c;
    // points_20[0].x=-c;

    points_01[0].x = 0 * cos_angle - 0 * sin_angle;
    points_01[0].y = 0 * sin_angle + 0 * cos_angle;

    points_12[0].x = (-0.5 * c) * cos_angle - 0.866 * c * sin_angle;
    points_12[0].y = (-0.5 * c) * sin_angle + 0.866 * c * cos_angle;

    points_20[0].x = -c * cos_angle - 0 * sin_angle;
    points_20[0].y = -c * sin_angle + 0 * cos_angle;

    points_20[1] = points_01[0];
    points_01[1] = points_12[0];
    points_12[1] = points_20[0];

    static Point draw_points = {0, 0};

    static int step = A; // 用于指示哪一段曲线a,b,c
    /*绘图坐标系下 以图形右下角为原点 （0，0）
        1
        /\
       /  \
    b /    \a
   2 /______\ 0
        c
    */
    switch (step)
    {
    case A:
        draw_line(&points_01[0], &points_01[1]);
        step = B;
        break;
    case B:
        draw_line(&points_12[0], &points_12[1]);
        step = C;
        break;
    case C:
        draw_line(&points_20[0], &points_20[1]);
        step = OVER;
        break;
    default:
        // LOG_E("step error case");
        break;
    }

    return 0;
}

float r;
int draw_cricle(void)
{
    float offset = 0;
    static Point draw_points = {0};
    // r = sqrt(pow(imagecenter.x - rightbottom.x, 2) + pow(imagecenter.y - rightbottom.y, 2));
    r = 0.1;
    static float delta_angle = 0.01; // 变化角度
    static float angle = 0;

    while (1)
    {
        angle += delta_angle;
        if (angle >= 2 * 3.1415926)
            angle = 2 * 3.1415926;
        draw_points.x = r * cos(angle);
        draw_points.y = r * sin(angle);

        // draw_points.x += imagecenter.x;
        // draw_points.y += imagecenter.y;

        graphics2corexy(&now_points, &draw_points);
        corexy_absolute_move(&corexy, now_points.x, now_points.y);

        offset = max(fabs(now_points.y - corexy.y), fabs(now_points.x - corexy.x));
        gap_time = fabs((offset / ((motor_vel * 0.04f) / 60.f)) * 100); // 有两个gap_time延时,这里的是为了不要快速的去算插值，导致跳跃很大。 还有一个延时（drv emm v5），是让电机运动到目标位置，再开始下一段插值。
        // rt_thread_mdelay(gap_time);
        Emm_V5_Pos_moveok();
        if (fabs(angle - 360) > 0.001f)
        {
            return 0;
        }
    }
    return 0;
}

// 以知道矩形中心点，矩形右下角顶点坐标 //均在绘图坐标系
int draw_square_rotate(void)
{
#define delta 0.001

    static float draw_x = 0;
    // static float tmp = 0;
    // tmp = sqrt( pow((imagecenter.x-rightbottom.x),2) + pow((imagecenter.y-rightbottom.y),2) );
    static float c = 0; // 边长
    // c = tmp/sqrt(2);
    c = 0.25 / 2;
    float offset = 0;       // 每次插值移动的距离
    float angle = 3.14 / 4; /*0--2*pi*/
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    float tmp0 = cos_angle - sin_angle;
    float tmp1 = sin_angle + cos_angle;
    // static Point points_01[2]={{0,0},{0,c}};
    // static Point points_12[2]={{0,c},{-c,c}};
    // static Point points_23[2]={{-c,c},{-c,0}};
    // static Point points_30[2]={{-c,0},{0,0}};

    static Point points_01[2] = {0};
    static Point points_12[2] = {0};
    static Point points_23[2] = {0};
    static Point points_30[2] = {0};

    // points_01[1].y=c;
    // points_12[0].y=c; points_12[1].x=-c; points_12[1].y=c;
    // points_23[0].x=-c; points_23[0].y=c; points_23[1].x=-c;
    // points_30[0].x=-c;

    ////////////////////////////////////////////////////////////
    points_01[0].x = 0 * cos_angle - 0 * sin_angle;
    points_01[0].y = 0 * sin_angle + 0 * cos_angle;

    points_12[0].x = 0 * cos_angle - c * sin_angle;
    points_12[0].y = 0 * sin_angle + c * cos_angle;

    points_23[0].x = -c * cos_angle - c * sin_angle;
    points_23[0].y = -c * sin_angle + c * cos_angle;

    points_30[0].x = -c * cos_angle - 0 * sin_angle;
    points_30[0].y = -c * sin_angle + 0 * cos_angle;

    points_30[1] = points_01[0];
    points_01[1] = points_12[0];
    points_12[1] = points_23[0];
    points_23[1] = points_30[0];

    LOG_D("point1 x1=%f; y1=%f;", points_01[0].x, points_01[0].y);
    LOG_D("point2 x2=%f; y2=%f;", points_12[0].x, points_12[0].y);
    LOG_D("point3 x3=%f; y3=%f;", points_23[0].x, points_23[0].y);
    LOG_D("point4 x4=%f; y4=%f;", points_30[0].x, points_30[0].y);
    ////////////////////////////////////////////////////////////

    static Point draw_points = {0, 0};

    static int step = A; // 用于指示哪一段曲线a,b,c,d
    /*绘图坐标系下 以图形右下角为原点 （0，0）
   2____b____1
    |       |
   c|       |a
   3|___d___|0
    */

    switch (step)
    {
    case A:
        draw_line(&points_01[0], &points_01[1]);
        step = B;
        break;
    case B:
        draw_line(&points_12[0], &points_12[1]);
        step = C;
        break;
    case C:
        draw_line(&points_23[0], &points_23[1]);
        step = D;
        break;
    case D:
        draw_line(&points_30[0], &points_30[1]);
        step = OVER;
        break;
    default:
        // LOG_E("step error case");
        break;
    }

    return 0;
}

static int state = 0; /* 0：归中点 1：运动到达第一个图形边点2：进行图形运动3：等待其他操作*/
void rbmg_handle(void *param)
{
    rt_thread_mdelay(1000);
    APID_Init(&pid_centerx, PID_POSITION, 0.0005f, 0, 0);
    APID_Init(&pid_centery, PID_POSITION, 0.0005f, 0, 0);

    rt_thread_mdelay(2000);

    corexy.x = 0.3;
    corexy.y = 0.3;

    while (1)
    {
        now_points.x = corexy.x;
        now_points.y = corexy.y;

        // draw_square();
        // draw_triangle();
        // draw_cricle();
        //  draw_square_rotate();
        // draw_triangle_rotate();

        // 摄像头 640*480    320 240
        // corexy 0.25 0.25

        if (state == 0)//归中间点
        {
            APID_Set_Present(&pid_centerx, now_datax);
            APID_Hander(&pid_centerx, 5);
            pid_outx = APID_Get_Out(&pid_centerx);

            APID_Set_Present(&pid_centery, now_datay);
            APID_Hander(&pid_centery, 5);
            pid_outy = APID_Get_Out(&pid_centery);
        }

        if (state == 1)//
        {
            APID_Set_Present(&pid_centerx, now_datax);
            APID_Hander(&pid_centerx, 5);
            pid_outx = APID_Get_Out(&pid_centerx);

            APID_Set_Present(&pid_centery, now_datay);
            APID_Hander(&pid_centery, 5);
            pid_outy = APID_Get_Out(&pid_centery);
        }

        if (state == 2)
        {
            APID_Set_Present(&pid_centerx, now_datax);
            APID_Hander(&pid_centerx, 5);
            pid_outx = APID_Get_Out(&pid_centerx);

            APID_Set_Present(&pid_centery, now_datay);
            APID_Hander(&pid_centery, 5);
            pid_outy = APID_Get_Out(&pid_centery);
        }

        if (state == 3)
        {
            APID_Set_Present(&pid_centerx, now_datax);
            APID_Hander(&pid_centerx, 5);
            pid_outx = APID_Get_Out(&pid_centerx);

            APID_Set_Present(&pid_centery, now_datay);
            APID_Hander(&pid_centery, 5);
            pid_outy = APID_Get_Out(&pid_centery);
        }
        rt_thread_mdelay(5);
    }
}

int rbmg_init(void)
{
    rt_thread_t tid_rbmg = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_rbmg = rt_thread_create("robotmanger",
                                rbmg_handle, RT_NULL,
                                4096,
                                16, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_rbmg != RT_NULL)
    {
        rt_thread_startup(tid_rbmg);
    }
    return 0;
}
INIT_APP_EXPORT(rbmg_init);