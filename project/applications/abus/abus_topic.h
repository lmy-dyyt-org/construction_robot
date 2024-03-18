/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-07 19:35:50
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-18 13:27:25
 * @FilePath: \abus_v2\abus_topic.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef ABUS_TOPIC_H
#define ABUS_TOPIC_H
#ifdef __cpluscplus
extern "C"
{
#endif

#define ABUS_TOPIC_STATIC(name, _struct_)

#include "cvector.h"
#include "alist.h"
#include "afifo.h"
	typedef struct abus_accounter abus_accounter_t;
	typedef struct abus_topic abus_topic_t;
	typedef int (*sub_callback)(abus_topic_t *sub);
	typedef struct
	{
		uint8_t is_async;
		uint8_t is_self_fifo;

	} abus_sub_flag;
	typedef struct abus_accounter
	{
		const char *name;
		alist_head_t head;
		afifo_t *datafifo;
		sub_callback callback;
		abus_sub_flag flag;
		abus_topic_t *topic;
	} abus_accounter_t;
	typedef struct
	{
		const char *name;
		afifo_t *datafifo;
		sub_callback callback;
	} abus_acc_init_t;
	typedef struct abus_topic
	{
		const char *name;
		alist_head_t sync_accounter;  // 同步订阅，在发布者支持同步条件下会同步调用
		alist_head_t async_accounter; // 由topic线程转发的
		afifo_t *datafifo;			  // 用于给发布者缓存数据，这个fifo支持覆盖
		uint32_t msg_size;

	} abus_topic_t;
	typedef struct
	{
		const char *name;
		uint8_t *buf;
		uint32_t buf_size;
		uint32_t msg_size;
	} abus_topic_init_t;

#define ABUS_ASSERT(x) \
	if (!(x))          \
		while (1)
	int abus_topic_init(abus_topic_t *topic, abus_topic_init_t *init);
	int abus_acc_init(abus_accounter_t *acc, abus_acc_init_t *init);
	int abus_topic_subscribe(abus_topic_t *topic, abus_accounter_t *acc, abus_sub_flag flag);
	int abus_topic_unsubscribe(abus_topic_t *topic, abus_accounter_t *acc);
	int abus_public(abus_accounter_t *acc, void *msg);
#ifdef __cpluscplus
}
#endif
#endif