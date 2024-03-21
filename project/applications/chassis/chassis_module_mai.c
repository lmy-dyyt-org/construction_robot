#include "chassis_module_mai.h"

#define DBG_TAG    "Chassis.mai"
#define DBG_LVL               DBG_INFO
#include <rtdbg.h>
#ifdef CHASSIS_MODULE_MAI
int module_mai(struct chassis *chassis, const void *output, const void *input);
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_mai(const void *output, const void *input);
#endif

chassis_mai_data_t mai_data;

chassis_ops_t ops_mai = {
    .module = module_mai,
#ifdef CHASSIS_USING_MOTOR_HAL
    .driver = driver_mai,
#else
    .driver = NULL,
#endif
    .date = &mai_data};

#define CHASSIS_MAI_WHELL_R_M 0.04
#define CHSSIS_MAI_A_M 0.218
#define CHSSIS_MAI_B_M 0.24
#define CHASSIS_HALF_A_B (0.5 * (CHSSIS_MAI_A_M + CHSSIS_MAI_B_M))
#define COS45 (0.70710678118)
#define SIN45 (0.70710678118)
#define CHASSIS_2PIR (6.28318530718 * CHASSIS_MAI_WHELL_R_M)
int module_mai(struct chassis *chassis, const void *output, const void *input)
{
    if (output != NULL)
    {
        // 读取底盘数据输出
        chassis_mai_data_t *data = (chassis_mai_data_t *)output;
        data->type = chassis->run_status;
        switch (chassis->run_status)
        {
        case CHASSIS_SPEED:
            // 速度控制
            // 将速度(m/s)转换为电机数据(rpm/min) 使用车轮半径和车体尺寸
            data->motor1 = ((((chassis->target.speed.x_m_s + chassis->target.speed.y_m_s) * 60.f)) * COS45 - (chassis->target.speed.z_rad_s * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor2 = ((((chassis->target.speed.x_m_s - chassis->target.speed.y_m_s) * 60.f)) * SIN45 - (chassis->target.speed.z_rad_s * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor3 = ((((-chassis->target.speed.x_m_s - chassis->target.speed.y_m_s) * 60.f)) * COS45 - (chassis->target.speed.z_rad_s * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor4 = ((((-chassis->target.speed.x_m_s + chassis->target.speed.y_m_s) * 60.f)) * SIN45 - (chassis->target.speed.z_rad_s * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            break;
        case CHASSIS_POS:
            // 位置控制
            // 将位置(m)转换为电机数据(rpm/min) 使用车轮半径和车体尺寸
            data->motor1 = ((((chassis->target.pos.x_m + chassis->target.pos.y_m) * 60.f)) / COS45 - (chassis->target.pos.z_rad * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor2 = ((((chassis->target.pos.x_m - chassis->target.pos.y_m) * 60.f)) / SIN45 - (chassis->target.pos.z_rad * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor3 = ((((-chassis->target.pos.x_m - chassis->target.pos.y_m) * 60.f)) / COS45 - (chassis->target.pos.z_rad * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;
            data->motor4 = ((((-chassis->target.pos.x_m + chassis->target.pos.y_m) * 60.f)) / SIN45 - (chassis->target.pos.z_rad * CHASSIS_HALF_A_B)) / CHASSIS_2PIR;

            break;
        default:
            break;
        }
    }
    if (input != NULL)
    {
        // 写入底盘数据
        chassis_mai_data_t *data = (chassis_mai_data_t *)input;

        switch (chassis->run_status)
        {
        case CHASSIS_SPEED:
            // 速度控制
            // 将电机数据(rpm/min)转换为速度(m/s) 使用车轮半径和车体尺寸
            chassis->present.speed.x_m_s = ((data->motor3 + data->motor4) * CHASSIS_2PIR / 60.f) / 2.f;
            chassis->present.speed.y_m_s = ((data->motor3 - data->motor1) * CHASSIS_2PIR / 60.f) / 2.f;
            chassis->present.speed.z_rad_s = ((data->motor2 - data->motor3) * CHASSIS_2PIR / 60.f) / (2.f * (CHSSIS_MAI_A_M + CHSSIS_MAI_B_M));
            break;
        case CHASSIS_POS:
            // 位置控制
            // 将电机数据(rpm/min)转换为位置(m) 使用车轮半径和车体尺寸
            chassis->present.pos.x_m = ((data->motor3 + data->motor4) * CHASSIS_2PIR / 60.f) / 2.f;
            chassis->present.pos.y_m = ((data->motor3 - data->motor1) * CHASSIS_2PIR / 60.f) / 2.f;
            chassis->present.pos.z_rad = ((data->motor2 - data->motor3) * CHASSIS_2PIR / 60.f) / (2.f * (CHSSIS_MAI_A_M + CHSSIS_MAI_B_M));

            break;
        default:

            break;
        }
        return 0;
    }
    return 0;
}
#ifdef CHASSIS_USING_MOTOR_HAL
static int driver_mai(const void *output, const void *input)
{
    if (input != NULL)
    {
        // 读取电机数据输出
        chassis_mai_data_t *data = (chassis_mai_data_t *)input;
        switch (data->type)
        {
        case CHASSIS_SPEED:
            // 速度控制
            //LOG_D("speed get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            data->motor1 = motor_get_speed(MOTOR_MAI_ID_1);
            data->motor2 = motor_get_speed(MOTOR_MAI_ID_2);
            data->motor3 = motor_get_speed(MOTOR_MAI_ID_3);
            data->motor4 = motor_get_speed(MOTOR_MAI_ID_4);
            break;
        case CHASSIS_POS:
            // 位置控制
            //LOG_D("pos get motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            data->motor1 = motor_get_pos(MOTOR_MAI_ID_1);
            data->motor2 = motor_get_pos(MOTOR_MAI_ID_2);
            data->motor3 = motor_get_pos(MOTOR_MAI_ID_3);
            data->motor4 = motor_get_pos(MOTOR_MAI_ID_4);
            break;
        default:
            break;
        }
    }
    if (output != NULL)
    {
        // 写入电机数据
        chassis_mai_data_t *data = (chassis_mai_data_t *)output;
        switch (data->type)
        {
        case CHASSIS_SPEED:
            // 速度控制
            //LOG_D("speed set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_speed(MOTOR_MAI_ID_1, data->motor1);
            motor_set_speed(MOTOR_MAI_ID_2, data->motor2);
            motor_set_speed(MOTOR_MAI_ID_3, data->motor3);
            motor_set_speed(MOTOR_MAI_ID_4, data->motor4);
            break;
        case CHASSIS_POS:

            // 位置控制
            //LOG_D("pos set motor1:%f motor2:%f motor3:%f motor4:%f\n", data->motor1, data->motor2, data->motor3, data->motor4);
            motor_set_pos(MOTOR_MAI_ID_1, data->motor1);
            motor_set_pos(MOTOR_MAI_ID_2, data->motor2);
            motor_set_pos(MOTOR_MAI_ID_3, data->motor3);
            motor_set_pos(MOTOR_MAI_ID_4, data->motor4);

            break;
        default:
            break;
        }
    }
    return 0;
}
#endif
#endif
