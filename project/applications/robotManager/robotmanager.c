#include "robotManager.h"
#include "abus_topic.h"

int rbmg_error_callback(abus_topic_t *sub)
{
    return 0;
}
int rbmg_dir_callback(abus_topic_t *sub)
{
    return 0;
}
int rbmg_special_point_callback(abus_topic_t *sub)
{
    return 0;
}
int rbmg_chassis_ctrl_callback(abus_topic_t *sub)
{
    return 0;
}