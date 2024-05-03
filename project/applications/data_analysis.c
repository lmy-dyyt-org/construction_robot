#include "data_analysis.h"
#include "string.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

#include <rtdbg.h>
#define DBG_TAG "drv.visio"
#define DBG_LVL DBG_DBG

struct frame_format data_frame_struct = // 数据帧结构体定义和初始化
	{
		.frame_start = '@',
		.frame_lenght = 0,
		.frame_end = '#',
		.data_start = '<',
		.data_lenght = 0,
		.data_end = '>',
};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     字符串转整形数字 数据范围是 [-32768,32767]
// 参数说明     *str            传入字符串 可带符号
// 返回参数     int32           转换后的数据
// 使用示例     int32 dat = func_str_to_int("-100");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
int32_t func_str_to_int(char *str)
{
	uint8_t sign = 0; // 标记符号 0-正数 1-负数
	int32_t temp = 0; // 临时计算变量
	do
	{
		if (NULL == str)
		{
			break;
		}

		if ('-' == *str) // 如果第一个字符是负号
		{
			sign = 1; // 标记负数
			str++;
		}
		else if ('+' == *str) // 如果第一个字符是正号
		{
			str++;
		}

		while (('0' <= *str) && ('9' >= *str)) // 确定这是个数字
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // 计算数值
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
// 函数简介     字符串转整形数字 数据范围是 [0,65535]
// 参数说明     *str            传入字符串 无符号
// 返回参数     uint32          转换后的数据
// 使用示例     uint32 dat = func_str_to_uint("100");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint32_t func_str_to_uint(char *str)
{
	uint32_t temp = 0; // 临时计算变量

	do
	{
		if (NULL == str)
		{
			break;
		}

		while (('0' <= *str) && ('9' >= *str)) // 确定这是个数字
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // 计算数值
			str++;
		}
	} while (0);

	return temp;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     字符串转浮点数 有效累计精度为小数点后六位
// 参数说明     *str            传入字符串 可带符号
// 返回参数     float           转换后的数据
// 使用示例     float dat = func_str_to_float("-100.2");
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float func_str_to_float(char *str)
{
	uint8_t sign = 0;		// 标记符号 0-正数 1-负数
	float temp = 0.0;		// 临时计算变量 整数部分
	float temp_point = 0.0; // 临时计算变量 小数部分
	float point_bit = 1;	// 小数累计除数

	do
	{
		if (NULL == str)
		{
			break;
		}

		if ('-' == *str) // 负数
		{
			sign = 1; // 标记负数
			str++;
		}
		else if ('+' == *str) // 如果第一个字符是正号
		{
			str++;
		}

		// 提取整数部分
		while (('0' <= *str) && ('9' >= *str)) // 确定这是个数字
		{
			temp = temp * 10 + ((uint8_t)(*str) - 0x30); // 将数值提取出来
			str++;
		}
		if ('.' == *str)
		{
			str++;
			while (('0' <= *str) && ('9' >= *str) && point_bit < 1000000.0) // 确认这是个数字 并且精度控制还没到六位
			{
				temp_point = temp_point * 10 + ((uint8_t)(*str) - 0x30); // 提取小数部分数值
				point_bit *= 10;										 // 计算这部分小数的除数
				str++;
			}
			temp_point /= point_bit; // 计算小数
		}
		temp += temp_point; // 将数值拼合

		if (sign)
		{
			temp = -temp;
		}
	} while (0);
	return temp;
}

// 判断字符串形式的数据的实际类型
// 返回值：无符号整数为0
// 有符号整数为1
// 浮点数为2
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

// 从提取的到所有字符里面提取出数据，并存进结构体得数据BUFF里面
void get_data_from_text(char *text)
{
	uint8_t data_flag = 0;
	// 清空实际数据长度
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

// // 接收回调函数
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
// 	static uint8_t receive_flag = 0;
// 	if (huart->Instance == USART1)
// 	{
// 		// 帧头，开始接收

// 		if (receive_flag == 0 && rxdata == data_frame_struct.frame_start)
// 		{
// 			memset(data_frame_struct.frame_text, '\0', 100);
// 			data_frame_struct.frame_lenght = 0;
// 			receive_flag = 1;
// 		}
// 		// 数据接收部分
// 		else if (receive_flag == 1 && rxdata != data_frame_struct.frame_end)
// 		{
// 			data_frame_struct.frame_text[data_frame_struct.frame_lenght] = rxdata;
// 			data_frame_struct.frame_lenght++;
// 		}
// 		// 帧尾，停止接收
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
/* 接收数据回调函数 */
static rt_err_t Visual_uart_receive_callback1(rt_device_t dev, rt_size_t size)
{

	// 发送信号量给线程
	data_size = size; // size是缓冲区现有字节个数
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
		// 	/////////////////////////// 数据读取
		while (data_size != 0)
		{
			if (rt_device_read(Visual_serial, 0, &rxdata, 1))
			{
				data_size--;
				// 没有数据了
				// 等待信号量,等待有数据
				// LOG_D("data receive :%c", rxdata);

				if (receive_flag == 0 && rxdata == data_frame_struct.frame_start)
				{
					memset(data_frame_struct.frame_text, '\0', 100);
					data_frame_struct.frame_lenght = 0;
					receive_flag = 1;
				}
				// 数据接收部分
				else if (receive_flag == 1 && rxdata != data_frame_struct.frame_end)
				{
					data_frame_struct.frame_text[data_frame_struct.frame_lenght] = rxdata;
					data_frame_struct.frame_lenght++;
				}
				// 帧尾，停止接收
				else if (receive_flag == 1 && rxdata == data_frame_struct.frame_end)
				{
					get_data_from_text((char *)data_frame_struct.frame_text);
					receive_flag = 0;
					LOG_D("data receive ok!");
				}
			}
			
			////////////////////数据打印
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
	// 创建信号量
	rt_sem_init(&Visual_sem1, "Visual_sem", 0, RT_IPC_FLAG_FIFO);
	// rt_kprintf("Visual_process\n");
	// /* 初始化静态互斥量 */
	// rt_err_t result = rt_mutex_init(&static_Visual_mutex1, "Visual_mutex", RT_IPC_FLAG_FIFO);
	// if (result != RT_EOK)
	// {
	// 	rt_kprintf("init static mutex failed.\n");
	// 	return -1;
	// }
	// servo1.mutex = &static_Visual_mutex1;

	// 初始化串口
	Visual_serial = rt_device_find("uart7");
	if (Visual_serial == RT_NULL)
	{
		rt_kprintf("Can't find uart device\n");
		return -1;
	}
	rt_device_open(Visual_serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(Visual_serial, Visual_uart_receive_callback1);
	// servo1.rx_size = 0;
	// 创建线程
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
