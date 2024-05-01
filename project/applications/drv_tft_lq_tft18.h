/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-05-01 10:57:14
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-05-01 10:57:47
 * @FilePath: \project\applications\drv_tft_lq_tft18.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2018年2月1日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】IAR8.20.1及以上版本
【Target 】 i.MX RT1052
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 528MHz
【USB PLL】 480MHz
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#ifndef __LQ_SGP18T_TFTSPI_H__
#define __LQ_SGP18T_TFTSPI_H__	
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

/*******************接口定义******************************/
#define TFT18W        162
#define TFT18H        132

#define	u16RED		0xf800
#define	u16GREEN	0x07e0
#define	u16BLUE		0x001f
#define	u16PURPLE	0xf81f
#define	u16YELLOW	0xffe0
#define	u16CYAN		0x07ff 		//蓝绿色
#define	u16ORANGE	0xfc08
#define	u16BLACK	0x0000
#define	u16WHITE	0xffff


/*****************私有函数声名*********************************/							
void TFTSPI_Init(uint8_t type);									 //LCD初始化  0:横屏  1：竖屏
void TFTSPI_Write_Cmd(uint8_t cmd);						         //发送控制字
void TFTSPI_Write_Byte(uint8_t dat);						         //发送数据参数
void TFTSPI_Write_Word(uint16_t dat);						         //发送像素显示参数
void TFTSPI_Addr_Rst(void);							         //DDRAM地址重置
void TFTSPI_Set_Pos(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye);	                         //定位显示像素位置
void TFTSPI_CLS(uint16_t color);					                         //全屏显示某种颜色
void TFTSPI_Draw_Part(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye,uint16_t color_dat);     //填充矩形区域，行起始、终止坐标，列起始、终止坐标，颜色
void TFTSPI_Draw_Line(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye,uint16_t color_dat);     //画线，行起始、终止坐标，列起始、终止坐标，颜色
void TFTSPI_Draw_Rectangle(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye,uint16_t color_dat);//画矩形边框，行起始、终止坐标，列起始、终止坐标，颜色
void TFTSPI_Draw_Circle(uint8_t x,uint8_t y,uint8_t r,uint16_t color_dat);                 //画圆形边框，圆心横坐标、纵坐标，半径，颜色
void TFTSPI_Draw_Dot(uint8_t x,uint8_t y,uint16_t color_dat);	                           //画点，横坐标，纵坐标，颜色
void TFTSPI_P8X16(uint8_t x, uint8_t y, uint8_t c_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P8X16Str(uint8_t x, uint8_t y, char *s_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P6X8(uint8_t x, uint8_t y, uint8_t c_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P6X8Str(uint8_t x, uint8_t y, char *s_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P6X8NUM(uint8_t x, uint8_t y, uint16_t num, uint8_t num_bit,uint16_t word_color,uint16_t back_color);
void TFTSPI_P8X8(uint8_t x, uint8_t y, uint8_t c_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P8X8Str(uint8_t x, uint8_t y, char *s_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P8X8NUM(uint8_t x, uint8_t y, uint16_t num, uint8_t num_bit,uint16_t word_color,uint16_t back_color);
void TFTSPI_P16x16Str(uint8_t x,uint8_t y,char *s_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_P16x12Str(uint8_t x,uint8_t y,char *s_dat,uint16_t word_color,uint16_t back_color);
void TFTSPI_Show_Pic(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye,uint8_t *ppic);
void TFTSPI_Show_Pic2(uint8_t xs,uint8_t ys,uint8_t w,uint8_t h,uint8_t *ppic) ;
void TFTSPI_Test(void);
void ColumnarSetting(uint8_t x,uint8_t y, uint8_t w,uint8_t h,uint16_t fclolr,uint16_t bcolor);
void DrawingColumnar(uint8_t t,uint8_t duty,uint8_t cl);
void DrawSpectrum(void);
void Clear_Columnar(void);
#ifdef __cplusplus
}
#endif
#endif /*SGP18T_ILI9163B.h*/
