#include "bus_sbus.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
uint8_t   dbus_buf[DBUS_BUFLEN];
rc_info_t rc;

#include "bus_sbus.h"


#define myabs(x) x>0?x:-x


/**
  * @brief       handle received rc data
  * @param[out]  rc:   structure to save handled rc data
  * @param[in]   buff: the buff which saved raw rc data
  * @retval
  */
void rc_callback_handler(rc_info_t *rc, uint8_t *buff)
{
    int min=15;


#if DJI
    rc->ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
    rc->ch1 -= 1024;
    rc->ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
    rc->ch2 -= 1024;
    rc->ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
    rc->ch3 -= 1024;
    rc->ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
    rc->ch4 -= 1024;

    rc->ch5 = ((buff[5] >> 4) & 0x000C) >> 2;
    rc->ch6 = (buff[5] >> 4) & 0x0003;

#else
  	rc->ch1 =((uint16_t)buff[1])|((uint16_t)((buff[2]&0x07)<<8));
  	rc->ch2 =((uint16_t)((buff[2]&0xf8)>>3))|(((uint16_t)(buff[3]&0x3f))<<5);
  	rc->ch3 =((uint16_t)((buff[3]&0xc0)>>6))|((((uint16_t)buff[4])<<2))|(((uint16_t)(buff[5]&0x01))<<10);
  	rc->ch4 =((uint16_t)((buff[5]&0xfe)>>1))|(((uint16_t)(buff[6]&0x0f))<<7);
  	rc->ch5=((uint16_t)((buff[6]&0xf0)>>4))|(((uint16_t)(buff[7]&0x7f))<<4);
  	rc->ch6=((uint16_t)((buff[7]&0x80)>>7))|(((uint16_t)buff[8])<<1)|(((uint16_t)(buff[9]&0x03))<<9);
  	rc->ch7=((uint16_t)((buff[9]&0xfc)>>2))|(((uint16_t)(buff[10]&0x1f))<<6);
  	rc->ch8=((uint16_t)((buff[10]&0xe0)>>5))|(((uint16_t)(buff[11]))<<3);
  	rc->ch9=((uint16_t)buff[12])|(((uint16_t)(buff[13]&0x07))<<8);
  	rc->ch10=((uint16_t)((buff[13]&0xf8)>>3))|(((uint16_t)(buff[14]&0x3f))<<5);
  	rc->ch11=((uint16_t)((buff[14]&0xc0)>>6))|(((uint16_t)buff[15])<<2)|(((uint16_t)(buff[16]&0x01))<<10);
  	rc->ch12=((uint16_t)((buff[16]&0xfe)>>1))|(((uint16_t)(buff[17]&0x0f))<<7);
  	rc->ch13=((uint16_t)((buff[17]&0xf0)>>4))|(((uint16_t)(buff[18]&0x7f))<<4);
  	rc->ch14=((uint16_t)((buff[18]&0x80)>>7))|(((uint16_t)buff[19])<<1)|(((uint16_t)(buff[20]&0x03))<<9);
  	rc->ch15=((uint16_t)((buff[20]&0xfc)>>2))|(((uint16_t)(buff[21]&0x1f))<<6);
		rc->ch16=((uint16_t)((buff[21]&0xe0)>>5))|(((uint16_t)buff[22])<<3);
	
		rc->ch1 -=1024;
		rc->ch2 -=1024;
		rc->ch3 -=1024;
		rc->ch4 -=1024;
		rc->ch13 -=1024;
		rc->ch14 -=1024;
		rc->ch15 -=1024;
		rc->ch16 -= 1024;
		
		sw_judge(rc);

#endif
    if(rc->ch1<min && rc->ch1>-min) rc->ch1=0;
    if(rc->ch2<min && rc->ch2>-min) rc->ch2=0;
    if(rc->ch3<min && rc->ch3>-min) rc->ch3=0;
    if(rc->ch4<min && rc->ch4>-min) rc->ch4=0;


}



/**
  * @brief   initialize dbus uart device
  * @param
  * @retval
  */
void dbus_uart_init(void)
{
    /* open uart idle it */

}

void sw_judge(rc_info_t *rc)
{
	if(rc->ch5 >1600) rc->ch5 = 1; else if(rc->ch5 <400) rc->ch5 = 3; else rc->ch5 = 2;
	if(rc->ch6 >1600) rc->ch6 = 1; else if(rc->ch6 <400) rc->ch6 = 3; else rc->ch6 = 2;
	if(rc->ch7 >1600) rc->ch7 = 1; else if(rc->ch7 <400) rc->ch7 = 3; else rc->ch7 = 2;
	if(rc->ch8 >1600) rc->ch8 = 1; else if(rc->ch8 <400) rc->ch8 = 3; else rc->ch8 = 2;
	if(rc->ch9 >1600) rc->ch9 = 1; else if(rc->ch9 <400) rc->ch9 = 3; else rc->ch9 = 2;
	if(rc->ch10 >1600) rc->ch10 = 1; else if(rc->ch10 <400) rc->ch10 = 3; else rc->ch10 = 2;
	if(rc->ch11 >1600) rc->ch11 = 1; else if(rc->ch11 <400) rc->ch11 = 3; else rc->ch11 = 2;
	if(rc->ch12 >1600) rc->ch12 = 1; else if(rc->ch12 <400) rc->ch12 = 3; else rc->ch12 = 2;
};


const rc_info_t *get_remote_control_point(void)
{
    return &rc;
}
