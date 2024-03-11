/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-11 00:04:33
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-11 09:52:33
 * @FilePath: \abus_v2.0.1\abus_v2.0.1\abus_v2.0.1.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// abus_v2.0.1.cpp: 定义应用程序的入口点。
//

#include "abus_v2.0.1.h"
#include "abus_topic.h"
#include "stdio.h"

typedef struct{
	uint8_t a;
	uint8_t b;
}my_data;
int my_sub_callback(abus_topic_t* sub)
{
	my_data dd;
	afifo_out(sub->datafifo,&dd,sub->msg_size);
	printf("a:%d,b:%d\n",dd.a,dd.b);
	
	return 0;

}

//int main()
//{
//	printf("Hello CMake.");
//	abus_topic_t topic;
//	abus_topic_init_t init;
//	init.buf = (uint8_t*)malloc(1024);
//	init.buf_size = 1024;
//	init.msg_size = sizeof(my_data);
//	init.name = "test_topic";
//	abus_topic_init(&topic, &init);
//	abus_accounter_t acc;
//	acc.name = "test_acc";
//	acc.callback = my_sub_callback;
//	acc.datafifo = NULL;
//	acc.flag.is_sync = 1;
//	abus_topic_subscribe(&topic, &acc, acc.flag);
//	my_data data;
//	data.a = 1;
//	data.b = 2;
//	abus_public(&acc, &data);
//		printf("end CMake.");


//	return 0;
//}
