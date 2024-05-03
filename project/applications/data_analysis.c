#include "data_analysis.h"
#include "string.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

#include <rtdbg.h>
#define DBG_TAG "drv.visio"
#define DBG_LVL DBG_DBG

struct frame_format data_frame_struct = // ����֡�ṹ�嶨��ͳ�ʼ��
	{
		.frame_start = '@',
		.frame_lenght = 0,
		.frame_end = '#',
		.data_start = '<',
		.data_lenght = 0,
		.data_end = '>',
};

//-------------------------------------------------------------------------------------------------------------------
// �������     �ַ���ת�������� ���ݷ�Χ�� [-32768,32767]
// ����˵��     *str            �����ַ��� �ɴ�����
// ���ز���     int32           ת���������
// ʹ��ʾ��     int32 dat = func_str_to_int("-100");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
int32_t func_str_to_int(char *str)
{
	uint8_t sign = 0; // ��Ƿ��� 0-���� 1-����
	int32_t temp = 0; // ��ʱ�������
	do
	{
		if (NULL == str)
		{
			break;
		}

		if ('-' == *str) // �����һ���ַ��Ǹ���
		{
			sign = 1; // ��Ǹ���
			str++;
		}
		else if ('+' == *str) // �����һ���ַ�������
		{
			str++;
		}

		while (('0' <= *str) && ('9' >= *str)) // ȷ�����Ǹ�����
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // ������ֵ
			str++;
		}

		if (sign)
		{
			temp = -temp;
		}
	} while (0);
	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �ַ���ת�������� ���ݷ�Χ�� [0,65535]
// ����˵��     *str            �����ַ��� �޷���
// ���ز���     uint32          ת���������
// ʹ��ʾ��     uint32 dat = func_str_to_uint("100");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
uint32_t func_str_to_uint(char *str)
{
	uint32_t temp = 0; // ��ʱ�������

	do
	{
		if (NULL == str)
		{
			break;
		}

		while (('0' <= *str) && ('9' >= *str)) // ȷ�����Ǹ�����
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // ������ֵ
			str++;
		}
	} while (0);

	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     �ַ���ת������ ��Ч�ۼƾ���ΪС�������λ
// ����˵��     *str            �����ַ��� �ɴ�����
// ���ز���     float           ת���������
// ʹ��ʾ��     float dat = func_str_to_float("-100.2");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
float func_str_to_float(char *str)
{
	uint8_t sign = 0;		// ��Ƿ��� 0-���� 1-����
	float temp = 0.0;		// ��ʱ������� ��������
	float temp_point = 0.0; // ��ʱ������� С������
	float point_bit = 1;	// С���ۼƳ���

	do
	{
		if (NULL == str)
		{
			break;
		}

		if ('-' == *str) // ����
		{
			sign = 1; // ��Ǹ���
			str++;
		}
		else if ('+' == *str) // �����һ���ַ�������
		{
			str++;
		}

		// ��ȡ��������
		while (('0' <= *str) && ('9' >= *str)) // ȷ�����Ǹ�����
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // ����ֵ��ȡ����
			str++;
		}
		if ('.' == *str)
		{
			str++;
			while (('0' <= *str) && ('9' >= *str) && point_bit < 1000000.0) // ȷ�����Ǹ����� ���Ҿ��ȿ��ƻ�û����λ
			{
				temp_point = temp_point * 10 + ((uint8_t)(*str) - 0x30); // ��ȡС��������ֵ
				point_bit *= 10;										 // �����ⲿ��С���ĳ���
				str++;
			}
			temp_point /= point_bit; // ����С��
		}
		temp += temp_point; // ����ֵƴ��

		if (sign)
		{
			temp = -temp;
		}
	} while (0);
	return temp;
}

// �ж��ַ�����ʽ�����ݵ�ʵ������
// ����ֵ���޷�������Ϊ0
// �з�������Ϊ1
// ������Ϊ2
uint8_t determine_type(char *text)
{
	uint8_t point_flag = 0;
	uint8_t signed_flag = 0;
	while (*text != '\0')
	{
		if (*text == '-')
		{
			signed_flag = 1;
		}
		if (*text == '.')
		{
			point_flag = 1;
		}
		text++;
	}
	if (point_flag != 0)
	{
		return 2;
	}
	if (signed_flag != 0)
	{
		return 1;
	}
	return 0;
}

// ����ȡ�ĵ������ַ�������ȡ�����ݣ�������ṹ�������BUFF����
void get_data_from_text(char *text)
{
	uint8_t data_flag = 0;
	// ���ʵ�����ݳ���
	data_frame_struct.data_buff.float_data_lenght = 0;
	data_frame_struct.data_buff.int32_data_lenght = 0;
	data_frame_struct.data_buff.uint32_data_lenght = 0;
	while (*text != '\0')
	{
		if (data_flag == 0 && *text == data_frame_struct.data_start)
		{
			memset(data_frame_struct.data_text, '\0', 10);
			data_frame_struct.data_lenght = 0;
			data_flag = 1;
		}
		else if (data_flag == 1 && *text != data_frame_struct.data_end)
		{
			// HAL_UART_Transmit(&huart2,(uint8_t *)"1",2,0xff);
			data_frame_struct.data_text[data_frame_struct.data_lenght] = *text;
			data_frame_struct.data_lenght++;
		}
		else if (data_flag == 1 && *text == data_frame_struct.data_end)
		{
			// HAL_UART_Transmit(&huart2,(uint8_t *)"2",2,0xff);
			uint8_t type = determine_type((char *)data_frame_struct.data_text);
			if (type == 0)
			{
				data_frame_struct.data_buff.uint32_data[data_frame_struct.data_buff.uint32_data_lenght] =
					func_str_to_uint((char *)data_frame_struct.data_text);
				data_frame_struct.data_buff.uint32_data_lenght++;
			}
			else if (type == 1)
			{
				data_frame_struct.data_buff.int32_data[data_frame_struct.data_buff.int32_data_lenght] =
					func_str_to_int((char *)data_frame_struct.data_text);
				data_frame_struct.data_buff.int32_data_lenght++;
			}
			else if (type == 2)
			{
				data_frame_struct.data_buff.float_data[data_frame_struct.data_buff.float_data_lenght] =
					func_str_to_float((char *)data_frame_struct.data_text);
				data_frame_struct.data_buff.float_data_lenght++;
			}
			data_flag = 0;
		}
		text++;
	}
	// HAL_UART_Transmit(&huart2,(uint8_t *)"3",2,0xff);
}

// // ���ջص�����
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
// 	static uint8_t receive_flag = 0;
// 	if (huart->Instance == USART1)
// 	{
// 		// ֡ͷ����ʼ����

// 		if (receive_flag == 0 && rxdata == data_frame_struct.frame_start)
// 		{
// 			memset(data_frame_struct.frame_text, '\0', 100);
// 			data_frame_struct.frame_lenght = 0;
// 			receive_flag = 1;
// 		}
// 		// ���ݽ��ղ���
// 		else if (receive_flag == 1 && rxdata != data_frame_struct.frame_end)
// 		{
// 			data_frame_struct.frame_text[data_frame_struct.frame_lenght] = rxdata;
// 			data_frame_struct.frame_lenght++;
// 		}
// 		// ֡β��ֹͣ����
// 		else if (receive_flag == 1 && rxdata == data_frame_struct.frame_end)
// 		{
// 			get_data_from_text((char *)data_frame_struct.frame_text);
// 			receive_flag = 0;
// 		}
// 		HAL_UART_Receive_IT(huart, &rxdata, 1);
// 	}
// }

struct rt_semaphore Visual_sem1;
rt_device_t Visual_serial;
uint8_t len = 0;
char str[10];
uint8_t rxdata;
uint8_t data_size;
uint8_t receive_flag = 0;
/* �������ݻص����� */
static rt_err_t Visual_uart_receive_callback1(rt_device_t dev, rt_size_t size)
{

	// �����ź������߳�
	data_size = size; // size�ǻ����������ֽڸ���
	// LOG_D("data come :%d!", data_size);
	rt_sem_release(&Visual_sem1);
	// LOG_D("data come :%d!", Visual_sem1.value);
	return 0;
}

void Visual_process(void *parameter)
{
	rt_thread_mdelay(1000);

	while (1)
	{
		rt_thread_mdelay(1);
		// LOG_D("data receive %d",Visual_sem1.value);
		// if ( rt_sem_take(&Visual_sem1, -1) > 0)
		// {
		// 	LOG_D("data receive %d",Visual_sem1.value);
		// 	/////////////////////////// ���ݶ�ȡ
		while (data_size != 0)
		{
			if (rt_device_read(Visual_serial, 0, &rxdata, 1))
			{
				data_size--;
				// û��������
				// �ȴ��ź���,�ȴ�������
				// LOG_D("data receive :%c", rxdata);

				if (receive_flag == 0 && rxdata == data_frame_struct.frame_start)
				{
					memset(data_frame_struct.frame_text, '\0', 100);
					data_frame_struct.frame_lenght = 0;
					receive_flag = 1;
				}
				// ���ݽ��ղ���
				else if (receive_flag == 1 && rxdata != data_frame_struct.frame_end)
				{
					data_frame_struct.frame_text[data_frame_struct.frame_lenght] = rxdata;
					data_frame_struct.frame_lenght++;
				}
				// ֡β��ֹͣ����
				else if (receive_flag == 1 && rxdata == data_frame_struct.frame_end)
				{
					get_data_from_text((char *)data_frame_struct.frame_text);
					receive_flag = 0;
					LOG_D("data receive ok!");
				}
			}
			
			////////////////////���ݴ�ӡ
			if (data_frame_struct.data_buff.float_data_lenght != 0)
			{
				LOG_D("float_data_lenght: %d", data_frame_struct.data_buff.float_data_lenght);
				for (uint8_t i = 0; i < data_frame_struct.data_buff.float_data_lenght; i++)
				{
					LOG_D("%f", data_frame_struct.data_buff.float_data[i]);
				}
			}

			if (data_frame_struct.data_buff.uint32_data_lenght != 0)
			{
				LOG_D("uint32_data_lenght: %d", data_frame_struct.data_buff.uint32_data_lenght);
				for (uint8_t i = 0; i < data_frame_struct.data_buff.uint32_data_lenght; i++)
				{
					LOG_D("%d", data_frame_struct.data_buff.uint32_data[i]);
				}
			}

			if (data_frame_struct.data_buff.int32_data_lenght != 0)
			{
				LOG_D("int32_data_lenght: %d", data_frame_struct.data_buff.int32_data_lenght);
				for (uint8_t i = 0; i < data_frame_struct.data_buff.int32_data_lenght; i++)
				{
					LOG_D("%d", data_frame_struct.data_buff.int32_data[i]);
				}
			}
		}
	}
}

int Visual_Init(void)
{
	rt_thread_t Visual_thread = RT_NULL;
	// �����ź���
	rt_sem_init(&Visual_sem1, "Visual_sem", 0, RT_IPC_FLAG_FIFO);
	// rt_kprintf("Visual_process\n");
	// /* ��ʼ����̬������ */
	// rt_err_t result = rt_mutex_init(&static_Visual_mutex1, "Visual_mutex", RT_IPC_FLAG_FIFO);
	// if (result != RT_EOK)
	// {
	// 	rt_kprintf("init static mutex failed.\n");
	// 	return -1;
	// }
	// servo1.mutex = &static_Visual_mutex1;

	// ��ʼ������
	Visual_serial = rt_device_find("uart7");
	if (Visual_serial == RT_NULL)
	{
		rt_kprintf("Can't find uart device\n");
		return -1;
	}
	rt_device_open(Visual_serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(Visual_serial, Visual_uart_receive_callback1);
	// servo1.rx_size = 0;
	// �����߳�
	Visual_thread = rt_thread_create("drv_Visual",
									 Visual_process, RT_NULL,
									 1024,
									 15, 1);

	if (Visual_thread != RT_NULL)
	{
		rt_thread_startup(Visual_thread);
	}

	return 0;
}
INIT_APP_EXPORT(Visual_Init);
