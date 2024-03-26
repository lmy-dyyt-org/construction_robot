
#include <rtthread.h>
#define DBG_TAG "RobotManager"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "robotManager.h"
#include "abus_topic.h"
#include "chassis_port.h"
#include "abus_topic.h"
#include "math.h"
#include "PathFinder.h"
#include "aboard_power_switch.h"

extern abus_accounter_t rbmg_error_acc;         // 接收error
extern abus_accounter_t rbmg_dir_acc;           // 发布dir
extern abus_accounter_t rbmg_special_point_acc; // 接收special point
extern abus_accounter_t rbmg_chassis_acc;       // 发布chassis ctrl

uint8_t rbmg_mode = CAB_MODE;
uint8_t chassis_dir = 0; // 车辆前进方向，以车体坐标系为主
static float line_error = 0;
static chassis_ctrl_t ctrl;

enum
{
    END = 0U,
    FORWARD,
    RIGHT,
    BACKWARD,
    LEFT,
    ROTATION,

};
uint8_t action_type;
uint8_t now_dir = 1;  // 当前方向
uint8_t next_dir = 1; // 下一个方向
Path_table_t *this_table;
Path_table_t test_go_table;
Path_table_element_t test_go[] = {FORWARD,END, END};

Path_table_t test_back_table;
Path_table_element_t test_back[] = {LEFT,END, END};

#define HALF_CAR_WIDTH 0.135f

void action_relative_movement_car(float _x_m, float _y_m, float _w_rad)
{
    extern chassis_t chassis_mai;
    const chassis_pos_t *nowpos = chassis_get_pos(&chassis_mai);

    // 打印电机位置
    // 发布底盘控制速度，前进
    ctrl.type = 1;
    ctrl.pos.x_m = nowpos->x_m + _x_m;
    ctrl.pos.y_m = nowpos->y_m + _y_m;
    ctrl.pos.z_rad = nowpos->z_rad + _w_rad;
    abus_public(&rbmg_chassis_acc, &ctrl);
    LOG_D("begin action");
    while (1)
    {
        if ((fabs(nowpos->y_m - ctrl.pos.y_m) < 0.01) && (fabs(nowpos->x_m - ctrl.pos.x_m) < 0.01) && (fabs(nowpos->z_rad - ctrl.pos.z_rad) < 0.1))
        {
            LOG_D("action over");
            break;
        }
        abus_public(&rbmg_chassis_acc, &ctrl);
        // LOG_D("[action]pos x:%f y:%f z:%f delta_x_m:%f delta_y_m:%f delta_w_rad:%f", nowpos->x_m, nowpos->y_m, nowpos->z_rad, _x_m, _y_m, _w_rad);
        rt_thread_mdelay(10);
    }
}

int special_action_1(void)
{
#define HALF_LINE 0.125f
    power_on(SWITCH_24V_4);
    //action_relative_movement_car(-HALF_LINE, 0, 0);
    action_relative_movement_car(0, 0, 0);

    extern float ymm;
    extern float zmm;

    /*抓*/
    ymm = 250;
    zmm = -25;
    rt_thread_mdelay(3000);

    ymm = 250;
    zmm = 15;
    rt_thread_mdelay(3000);

    //action_relative_movement_car(HALF_LINE, 0, 0);
    action_relative_movement_car(0.f, 0, 1.57 * 2.f);
    this_table = &test_back_table;
    rbmg_mode = LINE_MODE;
    LOG_D("special action 1 end now %s", this_table->name);
}

int special_action_2(void)
{
#define HALF_LINE 0.125f

    extern float ymm;
    extern float zmm;
    action_relative_movement_car(0.0, 0, 0.f);
    
    /*抓*/
    ymm = 250;
    zmm = -25;
    rt_thread_mdelay(3000);

    power_off(SWITCH_24V_4);
    ymm = 250;
    zmm = 15;
    rt_thread_mdelay(3000);

    // action_relative_movement_car(HALF_LINE, 0, 0);
    // action_relative_movement_car(0.f, 0, 1.57 * 2.f);
    // this_table = &test_back_table;

    rbmg_mode = CAB_MODE;
    LOG_D("special action 2 end now %s", this_table->name);
}

int turn_actions(uint8_t now_dir, uint8_t will_dir)
{
    int8_t delta_dir = will_dir - now_dir;

#define M_PI_2 (1.5707963267948966192313216916398)

    static float delta_angle = 0;

    switch (will_dir)
    {
    case END:
        if (this_table == &test_go_table)
        {
            special_action_1();
            return 0;
        }
        else if (this_table == &test_back_table)
        {
            // LOG_D("test_back_table end special action start");
            // LOG_D("test_back_table end special action end!");
            special_action_2();
            while (1)
            {
                LOG_D("stop");
                rt_thread_mdelay(100);
            }
        }
        break;
    case FORWARD:
        delta_angle = 0.001;
        break;
    case RIGHT:
        delta_angle = -M_PI_2;
        break;
    case BACKWARD:
        delta_angle = M_PI_2 * 2;
        break;
    case LEFT:
        delta_angle = M_PI_2;
        break;
    default:
        LOG_D("error dir");
        break;
    }
    LOG_D("rota start %f", delta_angle);

    action_relative_movement_car(0, HALF_CAR_WIDTH, 0);

    LOG_D("front end");
    LOG_D("rota start %f", delta_angle);

    action_relative_movement_car(0, 0, delta_angle);
    LOG_D("rota end");

    return 0;
}

/**
 * @brief 巡线使用的callback,注意要支持禁用操作
 *
 * @param sub
 * @return int
 */
int rbmg_error_callback(abus_topic_t *sub)
{
#define KK 1.1f // 1.3
#define SPEED 0.25
    if (rbmg_mode == LINE_MODE)
    {
        // 巡线模式下的处理
        afifo_out_data(sub->datafifo, (uint8_t *)&line_error, sizeof(float));
        // 发布控制到底盘
        ctrl.type = 0;
        chassis_dir = 0;
        switch (chassis_dir)
        {
        case 0: // 前
            ctrl.speed.x_m_s = 0;
            ctrl.speed.y_m_s = SPEED;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 1: // 右边
            ctrl.speed.x_m_s = SPEED;
            ctrl.speed.y_m_s = 0;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 2: // 后边
            ctrl.speed.x_m_s = 0;
            ctrl.speed.y_m_s = -SPEED;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        case 3: // 左边
            ctrl.speed.x_m_s = -SPEED;
            ctrl.speed.y_m_s = 0;
            ctrl.speed.z_rad_s = line_error * KK;
            break;
        }
        //        LOG_D("line mode");
        abus_public(&rbmg_chassis_acc, &ctrl);
    }

    return 0;
}

int rbmg_dir_callback(abus_topic_t *sub)
{
    // 接收dir变换
    return 0;
}
int rbmg_special_point_callback(abus_topic_t *sub)
{
    // 接收特殊点

    /**
     * @brief 对于特殊点的处理，询问寻路器，切换然后执行动作组最后动作组交回执行权
     * 注意不在回调中执行回调仅仅切换rbmg_mode,可以发送信号量
     *
     */
    if (rbmg_mode != CAB_MODE)
    {
        rbmg_mode = ACTION_MODE;
        LOG_D("special point! now action mode");
    }

    return 0;
}
int rbmg_chassis_ctrl_callback(abus_topic_t *sub)
{
    // 接收底盘控制数据
    return 0;
}

void rbmg_handle(void *parameter)
{
    // rbmg_mode = ACTION_MODE;
    // action_front_car(0.107f);
    while (1)
    {
        // LOG_D("rbmg he

        // 接到处理数据的消息
        if (rbmg_mode == CAB_MODE)
        {
            while (1)
            {
                LOG_D("cab mode");
                if (rbmg_mode != CAB_MODE)
                {

                    break;
                }
                rt_thread_mdelay(500);
            }
        }
        else if (rbmg_mode == LINE_MODE)
        {
            // 巡线都在回调中处理
            // LOG_D("line mode");
        }
        else if (rbmg_mode == ACTION_MODE)
        {
            // 动作模式下的处理
            // 完成动作后切换回巡线模式
            LOG_D("action start");
            // action_relative_movement_car(0, 0.135f, 0);
            // action_relative_movement_car(0, 0, 3.14f/2.0f);

            /* 更新寻路器*/
            now_dir = next_dir;
            next_dir = Path_get_next_dir(this_table);
            LOG_D("now dir %d next dir %d", now_dir, next_dir);
            // 判断当前是否寻路完成进行切换或者特殊action
            // if (now_dir == 0)
            //     while (1)
            //     {
            //         LOG_D("finder end");
            //         rt_thread_mdelay(500);
            //     }

            // 转弯或特殊action
            turn_actions(now_dir, next_dir);
            rbmg_mode = LINE_MODE;
            LOG_D("action completion");
        }
        rt_thread_mdelay(50);
    }
}

int rbmg_init(void)
{

    Path_table_init(&test_go_table, test_go, "test go table", 0, 0);
    Path_table_init(&test_back_table, test_back, "test back table", 0, 0);
    this_table = &test_go_table;
    rt_thread_t tid_rbmg = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_rbmg = rt_thread_create("robotmanger",
                                rbmg_handle, RT_NULL,
                                4096,
                                7, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_rbmg != RT_NULL)
    {
        rt_thread_startup(tid_rbmg);
    }
    return 0;
}
INIT_APP_EXPORT(rbmg_init);