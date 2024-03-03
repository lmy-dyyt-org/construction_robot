#include <board.h>
#include <rtthread.h>
#include <drv_gpio.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "ulog.h"
#define LED0_PIN    GET_PIN(F, 14)

enum{
 SWITCH_24V_1 =GET_PIN(H,5),  //PH5
 SWITCH_24V_2 =GET_PIN(H,4),	//PH4
 SWITCH_24V_3 =GET_PIN(H,2),	//Ph2
 SWITCH_24V_4 =GET_PIN(H,3),	//ph3
 SWITCH_5V_1  =GET_PIN(G,13), //PG13

};


int power_on(int id)
{
		rt_pin_write(id, PIN_HIGH);

		return 0;
}

int power_off(int id)
{
		rt_pin_write(id, PIN_LOW);

		return 0;
}

int power_init(void)
{
    rt_pin_mode(SWITCH_24V_1, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_2, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_3, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_24V_4, PIN_MODE_OUTPUT);
    rt_pin_mode(SWITCH_5V_1, PIN_MODE_OUTPUT);
	
	
	power_off(SWITCH_24V_1);
	power_off(SWITCH_24V_2);
	power_off(SWITCH_24V_3);
	power_off(SWITCH_24V_4);
	power_off(SWITCH_5V_1 );
	
	return 0;
}
INIT_BOARD_EXPORT(power_init);

