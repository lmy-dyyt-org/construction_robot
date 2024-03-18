#include "abus_topic.h"
#include <rtthread.h>
#include <ulog.h>

#include "chassis_port.h"

abus_topic_t line_error_topic;
abus_topic_t line_dir_topic;
abus_topic_t line_special_topic;

abus_topic_t chassis_ctrl_topic;

 abus_accounter_t line_error_acc;
 abus_accounter_t line_dir_acc;
 abus_accounter_t line_spacial_point_acc;

 abus_accounter_t chassis_acc;

//static abus_accounter_t line_chassis_ctrl_acc;

int abus_all_init(void)
{
    extern int line_dir_sub_callback (abus_topic_t* sub);
    extern int chassis_sub_callback (abus_topic_t* sub);

    
	abus_topic_init_t init;
	init.buf = (uint8_t*)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(float);
	init.name = "line_error_topic";
	abus_topic_init(&line_error_topic, &init);

	init.buf = (uint8_t*)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(uint8_t);
	init.name = "line_special_topic";
	abus_topic_init(&line_special_topic, &init);

	init.buf = (uint8_t*)malloc(1024);
	init.buf_size = 1024;
	init.msg_size = sizeof(uint8_t);
	init.name = "line_dir_topic";
	abus_topic_init(&line_dir_topic, &init);

	line_error_acc.name = "line_error_acc";
	line_error_acc.callback = NULL;
	line_error_acc.datafifo = NULL;
	line_error_acc.flag.is_async = 0;
	abus_topic_subscribe(&line_error_topic, &line_error_acc, line_error_acc.flag);
    // 初始化topic
    abus_topic_init_t topic_init;
    topic_init.name = "chassis_ctrl";
    topic_init.buf = rt_malloc(1024);
    topic_init.buf_size = 1024;
    topic_init.msg_size = sizeof(chassis_ctrl_t);
    abus_topic_init(&chassis_ctrl_topic, &topic_init);


	line_dir_acc.name = "line_dir_acc";
	line_dir_acc.callback = line_dir_sub_callback;
	line_dir_acc.datafifo = NULL;
	line_dir_acc.flag.is_async = 0;
	abus_topic_subscribe(&line_dir_topic, &line_dir_acc, line_dir_acc.flag);

	line_spacial_point_acc.name = "line_special_acc";
	line_spacial_point_acc.callback = NULL;
	line_spacial_point_acc.datafifo = NULL;
	line_spacial_point_acc.flag.is_async = 0;
	abus_topic_subscribe(&line_special_topic, &line_spacial_point_acc, line_spacial_point_acc.flag);


    // 初始化acc
    abus_acc_init_t acc_init;
    acc_init.name = "chassis_acc";
    acc_init.datafifo = NULL;
    acc_init.callback = chassis_sub_callback;
    abus_acc_init(&chassis_acc, &acc_init);
    // 订阅
    abus_topic_subscribe(&chassis_ctrl_topic, &chassis_acc, (abus_sub_flag){0, 0});
 

	// line_chassis_ctrl_acc.name = "line_chs_ctrl_acc";
	// line_chassis_ctrl_acc.callback = NULL;
	// line_chassis_ctrl_acc.datafifo = NULL;
	// line_chassis_ctrl_acc.flag.is_async = 0;
	// abus_topic_subscribe(&chassis_ctrl_topic, &line_chassis_ctrl_acc, line_chassis_ctrl_acc.flag);

}INIT_BOARD_EXPORT(abus_all_init);
