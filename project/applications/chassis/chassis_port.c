/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-16 21:52:49
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-17 21:02:55
 * @FilePath: \project\applications\chassis\chassis_port.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "chassis_port.h"
#include "chassis_module_mai.h"
#include "rtthread.h"
chassis_t chassis_mai;

chassis_speed_t chassis_speed;
chassis_pos_t chassis_pos;

void chassis_port_handle(void *parameter)
{
    // int chassis_set_speed(chassis_t *chassis, chassis_speed_t *data);
    // int chassis_set_pos(chassis_t *chassis, chassis_pos_t *data);
// chassis_speed.x_m_s = 10;
// chassis_speed.y_m_s = 10;
chassis_speed.z_rad_s = 1;

// chassis_pos.x_m = 10;
// chassis_pos.y_m = 10;   
chassis_pos.z_rad = 0;
    //chassis_set_speed(&chassis_mai, &chassis_speed);
    chassis_set_pos(&chassis_mai, &chassis_pos);
    while (1)
    {
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)
        chassis_handle(&chassis_mai, 0);
#endif

	rt_thread_mdelay(50);
    }
}
int chassis_port_init(void)
{
#if defined(CHASSIS_MODULE_MAI) && defined(CHASSIS_MODULE_MAI)
    chassis_init(&chassis_mai, &ops_mai);
#endif

    rt_thread_t tid_chassis = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_chassis = rt_thread_create("chassis_mai",
                                   chassis_port_handle, RT_NULL,
                                   4096,
                                   22, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_chassis != RT_NULL)
    {
        rt_thread_startup(tid_chassis);
    }
		return 0;
}
INIT_ENV_EXPORT(chassis_port_init);
