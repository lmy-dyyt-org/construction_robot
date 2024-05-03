#ifndef DATA_ANALYSIS_H
#define DATA_ANALYSIS_H

#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

extern struct frame_format data_frame_struct;

struct data_buff_struct // ����buff�ṹ�嶨��
{
	float float_data[10];	  // ������������
	uint32_t uint32_data[10]; // �޷����������ݴ洢
	int32_t int32_data[10];	  // �з����������ݴ洢

	uint8_t float_data_lenght;	// ���������ݳ���
	uint8_t uint32_data_lenght; // �޷����������ݳ���
	uint8_t int32_data_lenght;	// �з����������ݳ���
};

struct frame_format // ����֡�ṹ�嶨��
{
	uint8_t frame_text[100];		   // ����֡��Ч����
	uint8_t frame_start;			   // ����֡֡ͷ
	uint8_t frame_lenght;			   // ����֡����
	uint8_t frame_end;				   // ����֡֡β
	uint8_t data_text[10];			   // ������Ч����
	uint8_t data_start;				   // ����֡ͷ
	uint8_t data_lenght;			   // ���ݳ���
	uint8_t data_end;				   // ����֡β
	struct data_buff_struct data_buff; // ����buff�ṹ��
};

int32_t func_str_to_int(char *str);
uint32_t func_str_to_uint(char *str);
float func_str_to_float(char *str);

void get_data_from_text(char *text);

#endif
