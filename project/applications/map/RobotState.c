#include "RobotState.h"
#include "PathFinder.h"
#include "stdio.h"

void Robot_State_Trigger(Robote_State_t *robot_state)
{
    // 触发机器人请求寻路器并且更新位置且发送信息
    LOG_D("Robot_State_Trigger\r\n");
    extern uint8_t Path_get_next_dir(Path_table_t * table);
    int dir = Path_get_next_dir(robot_state->table);

    robot_state->now_state.x = robot_state->will_state.x;
    robot_state->now_state.y = robot_state->will_state.y;
    // 根据dir更新will_state
    Smt_map_t *state = &(robot_state->map[robot_state->now_state.x][robot_state->now_state.y]);
    switch (dir)
    {
    case 0:
        LOG_D("到达终点\r\n");
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
        LOG_E("error dir\r\n");
        break;
    }
    robot_state->will_state.x = 2;
    robot_state->will_state.y = 2;
    // 发布will更新消息
        //TODO:
        
    // 显示状态
    LOG_D("robot_state->now_state.x = %d\r\n", robot_state->now_state.x);
    LOG_D("robot_state->now_state.y = %d\r\n", robot_state->now_state.y);
    LOG_D("robot_state->will_state.x = %d\r\n", robot_state->will_state.x);
    LOG_D("robot_state->will_state.y = %d\r\n", robot_state->will_state.y);
}