#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

extern struct frame_format data_frame_struct;

struct data_buff_struct // 数据buff结构体定义
{
	float float_data[10];	  // 浮点数据数组
	uint32_t uint32_data[10]; // 无符号整型数据存储
	int32_t int32_data[10];	  // 有符号整型数据存储

	uint8_t float_data_lenght;	// 浮点数数据长度
	uint8_t uint32_data_lenght; // 无符号整型数据长度
	uint8_t int32_data_lenght;	// 有符号整型数据长度
};

struct frame_format // 数据帧结构体定义
{
	uint8_t frame_text[100];		   // 数据帧有效内容
	uint8_t frame_start;			   // 数据帧帧头
	uint8_t frame_lenght;			   // 数据帧长度
	uint8_t frame_end;				   // 数据帧帧尾
	uint8_t data_text[10];			   // 数据有效内容
	uint8_t data_start;				   // 数据帧头
	uint8_t data_lenght;			   // 数据长度
	uint8_t data_end;				   // 数据帧尾
	struct data_buff_struct data_buff; // 数据buff结构体
};

int32_t func_str_to_int(char *str);
uint32_t func_str_to_uint(char *str);
float func_str_to_float(char *str);

void get_data_from_text(char *text);

#endif
