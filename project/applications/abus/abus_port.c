#include "abus_topic.h"
#include <rtthread.h>
#include <ulog.h>

#include "chassis_port.h"
#include "robotmanager.h"

//机器人manager的acc
/**
 * @brief robot manager的acc
 * robot manager 是机器人的核心逻辑模块，负责整个机器人的状态管理，路径规划，任务分配等等
 * 
 */


// static abus_accounter_t line_chassis_ctrl_acc;

int abus_all_init(void)
{
	return 0;
}
INIT_BOARD_EXPORT(abus_all_init);
