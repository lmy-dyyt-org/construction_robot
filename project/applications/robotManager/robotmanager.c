/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-19 09:10:31
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-01 17:40:20
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

Point offset;

void corexy2graphics(const Point* corexy,Point* graphics)
{
    graphics->x = corexy->x + offset.x;
    graphics->y = corexy->y + offset.y;
}

void corexy2graphics(Point* corexy,const Point* graphics)
{
    corexy->x = graphics->x - offset.x;
    corexy->y = graphics->y - offset.y;
}

void draw_square(uint32_t time_ms,Point* out)
{
    
}
void rbmg_handle(void*param)
{
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