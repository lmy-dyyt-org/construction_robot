#ifndef ROBOTMANAGER_H
#define ROBOTMANAGER_H
#ifdef __cplusplus
extern "C" {
#endif

int rbmg_error_callback(abus_topic_t *sub);
int rbmg_dir_callback(abus_topic_t *sub);
int rbmg_special_point_callback(abus_topic_t *sub);
int rbmg_chassis_ctrl_callback(abus_topic_t *sub);

#ifdef __cplusplus
}
#endif
#endif /* ROBOTMANAGER_H */
