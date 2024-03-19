/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-19 09:02:04
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-19 09:09:49
 * @FilePath: \project\applications\map\RobotState.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "RobotState.h"
#include "PathFinder.h"
#include "stdio.h"

//由特殊点触发
void Robot_State_Trigger(Robote_State_t *robot_state)
{
    // 触发机器人请求寻路器并且更新位置且发送信息
    LOG_D("Robot_State_Trigger\r\n");
    extern uint8_t Path_get_next_dir(Path_table_t * table);
    int abs_dir = Path_get_next_dir(robot_state->table);

    robot_state->now_state.x = robot_state->will_state.x;
    robot_state->now_state.y = robot_state->will_state.y;
    // 根据dir更新will_state
    Smt_map_t *state = &(robot_state->map[robot_state->now_state.x][robot_state->now_state.y]);
    switch (abs_dir)
    {
    case 0:
        LOG_D("到达终点\r\n");
        //发布通知,展示到达终点，或者切换寻路器等等
        return;
    case 1:
        robot_state->will_state.x = robot_state->now_state.x - 1;
        robot_state->will_state.y = robot_state->now_state.y;
        break;
    case 2:

        robot_state->will_state.x = robot_state->now_state.x;
        robot_state->will_state.y = robot_state->now_state.y + 1;
        break;
    case 3:

        robot_state->will_state.x = robot_state->now_state.x + 1;
        robot_state->will_state.y = robot_state->now_state.y;
        break;
    case 4:

        robot_state->will_state.x = robot_state->now_state.x;
        robot_state->will_state.y = robot_state->now_state.y - 1;
        break;
    default:
        LOG_E("error abs_dir\r\n");
        break;
    }
    robot_state->will_state.x = 2;
    robot_state->will_state.y = 2;
    // 发布will更新消息,车辆执行转弯/直行等等
        //TODO:
        
    // 显示状态
    LOG_D("robot_state->now_state.x = %d\r\n", robot_state->now_state.x);
    LOG_D("robot_state->now_state.y = %d\r\n", robot_state->now_state.y);
    LOG_D("robot_state->will_state.x = %d\r\n", robot_state->will_state.x);
    LOG_D("robot_state->will_state.y = %d\r\n", robot_state->will_state.y);
}