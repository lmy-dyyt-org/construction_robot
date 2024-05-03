#include "drv_corexy.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#define DBG_TAG "corxy"
#define DBG_LVL DBG_LOG
#include "math.h"

corexy_t corexy;
corexy_t real_corexy;
int corexy_init(corexy_t *corexy)//归位 左下角 0，0
{
    corexy->x = 0;
    corexy->y = 0;
    return 0;
}

int corexy_relative_move(corexy_t *corexy,float x,float y)//x,y为增量
{
    corexy->x += x;
    corexy->y += y;
    return 0;
}

int corexy_absolute_move(corexy_t *corexy,float x,float y)//x,y为坐标
{
    corexy->x = x;
    corexy->y = y;
    return 0;
}

void corexy_entry(void *t)
{
    corexy_init(&corexy);

    // corexy.x = 0; //m
    // corexy.y = 0;
    while (1)
    {
        //视觉传入图像，计算插补



        rt_thread_mdelay(100);
    }
}

int drv_corexy_init(void)
{
    rt_thread_t corexy_tb = RT_NULL;

    corexy_tb = rt_thread_create("corexy",
                                     corexy_entry, RT_NULL,
                                     8192,
                                     15, 1);

    if (corexy_tb != RT_NULL)
    {
        rt_thread_startup(corexy_tb);
    }
    return 0;
}
INIT_APP_EXPORT(drv_corexy_init);



