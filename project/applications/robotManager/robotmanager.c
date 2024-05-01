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

Point offset;//corexy到绘图坐标系的偏移


Point rightbottom;//corexy坐标系下图像的右下角顶点


void corexy2graphics(const Point* corexy,Point* graphics)
{
    graphics->x = corexy->x + offset.x;
    graphics->y = corexy->y + offset.y;
}

void graphics2corexy(Point* corexy,const Point* graphics)
{
    corexy->x = graphics->x - offset.x;
    corexy->y = graphics->y - offset.y;
}
enum{
    A=0U,
    B, 
    C,
    D,
}

//以知道矩形中心点，矩形右下角顶点坐标 //均在绘图坐标系
int draw_square(uint32_t time_ms,Point* out)
{
    static Point points[4];
    static int step=A;//用于指示哪一段曲线a,b,c,d
    /*绘图坐标系下图像中心为原点00
   2____b____1
    |       |
   c|       |a
   3|___d___|0
    */
    if(time_ms == 0)
    {
        corexy2graphics(&rightbottom,points[0]);
        point[1].x = points[0].x;
        point[1].y = -points[0].y;

        point[2].x = -points[0].x;
        point[2].y = -points[0].y;

        point[3].x = -points[0].x;
        point[3].y = points[0].y;
    }
    switch(step)
    {
        case A:
            if(time_ms < 1000)
            {
                out->x += points[0].x;
                out->y += points[0].y;
                if(out->x>points[1].x)out->x=points[1].x;
                if(out->y>points[1].y)out->y=points[1].y;
                if(out->x == points[1].x && out->y == points[1].y)step=B;
            }else{
                step = B;
            }
        break;
    }
            
}
void rbmg_handle(void*param)
{
    while(1){
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