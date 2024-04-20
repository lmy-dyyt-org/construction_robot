#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#define DBG_TAG "drv.step_motor"
#define DBG_LVL DBG_LOG
#include "drv_emm_v5.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))
rt_mutex_t mutex_step = RT_NULL; // 互斥锁
rt_sem_t emm_rx_sem = RT_NULL;
rt_device_t Emm_serial1 = RT_NULL;
int Emm_rx_size = 0;
uint8_t Emm_rx_buf[64] = {0};

stepper_motor_t stepper11;

void emm_transmit(uint8_t *data, uint8_t len)
{
    rt_device_write(Emm_serial1, 0, data, len);
}

int emm_get_cmd_receive_size(uint8_t cmd)
{
    switch (cmd)
    {
    case 0x35: // v
        return 6;
    case 0x1F: // p
        return 8;
    default:
        return 4;
    }
}

void emm_cmd_ana(uint8_t cmd, uint8_t *buf)
{
    switch (cmd)
    {
    case 0x35: // v
        if (Emm_rx_buf[0] == 1 && Emm_rx_buf[1] == 0x35)
        {

            // 拼接成uint16_t类型数据
            uint16_t vel = (uint16_t)(((uint16_t)Emm_rx_buf[3] << 8) |
                                      ((uint16_t)Emm_rx_buf[4] << 0));

            stepper11.stepper_motor_speed = vel;

            // 符号
            if (Emm_rx_buf[2])
            {
                stepper11.stepper_motor_speed = -stepper11.stepper_motor_speed;
            }
            LOG_D("emm_cmd_ana v:%d", stepper11.stepper_motor_speed);
        }
        else
            return;
    case 0x1F: // p

        return;
    default:
        if (buf[2] != 0x6B)
            LOG_W("emm_cmd answer nuknown");
        return;
    }
}
void emm_wait_for_ack(uint8_t cmd)
{
    uint8_t will_rx_size = emm_get_cmd_receive_size(cmd);
    uint8_t Emm_rx_index = 0;
    uint8_t ch = 0;
    while (1)
    {
        while (rt_device_read(Emm_serial1, -1, &ch, 1) != 1)
        {
            rt_err_t ret = rt_sem_take(emm_rx_sem, 100);
            if (ret != RT_EOK)
            {
                LOG_E("emm_wait_for_ack timeout");
                rt_mutex_release(mutex_step);
                return;
            }
            else
            {
            }
        }
        // 获取到字符
        Emm_rx_buf[Emm_rx_index++] = ch;
        if (Emm_rx_index >= will_rx_size)
        {
            emm_cmd_ana(cmd, Emm_rx_buf);
            rt_mutex_release(mutex_step);
            return;
        }
    }

    rt_mutex_release(mutex_step);
}
/**
 * @brief    将当前位置清零
 * @param    addr  ：电机地址
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0A; // 功能码
    cmd[2] = 0x6D; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 4);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    解除堵转保护
 * @param    addr  ：电机地址
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Reset_Clog_Pro(uint8_t addr)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0E; // 功能码
    cmd[2] = 0x52; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 4);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    读取系统参数
 * @param    addr  ：电机地址
 * @param    s     ：系统参数类型
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
    uint8_t i = 0;
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[i] = addr;
    ++i; // 地址

    switch (s) // 功能码
    {
    case S_VER:
        cmd[i] = 0x1F;
        ++i;
        break;
    case S_RL:
        cmd[i] = 0x20;
        ++i;
        break;
    case S_PID:
        cmd[i] = 0x21;
        ++i;
        break;
    case S_VBUS:
        cmd[i] = 0x24;
        ++i;
        break;
    case S_CPHA:
        cmd[i] = 0x27;
        ++i;
        break;
    case S_ENCL:
        cmd[i] = 0x31;
        ++i;
        break;
    case S_TPOS:
        cmd[i] = 0x33;
        ++i;
        break;
    case S_VEL:
        cmd[i] = 0x35;
        ++i;
        break;
    case S_CPOS:
        cmd[i] = 0x36;
        ++i;
        break;
    case S_PERR:
        cmd[i] = 0x37;
        ++i;
        break;
    case S_FLAG:
        cmd[i] = 0x3A;
        ++i;
        break;
    case S_ORG:
        cmd[i] = 0x3B;
        ++i;
        break;
    case S_Conf:
        cmd[i] = 0x42;
        ++i;
        cmd[i] = 0x6C;
        ++i;
        break;
    case S_State:
        cmd[i] = 0x43;
        ++i;
        cmd[i] = 0x7A;
        ++i;
        break;
    default:
        break;
    }

    cmd[i] = 0x6B;
    ++i; // 校验字节

    // 发送命令
    emm_transmit(cmd, i);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    修改开环/闭环控制模式
 * @param    addr     ：电机地址
 * @param    svF      ：是否存储标志，false为不存储，true为存储
 * @param    ctrl_mode：控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, uint8_t ctrl_mode)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;      // 地址
    cmd[1] = 0x46;      // 功能码
    cmd[2] = 0x69;      // 辅助码
    cmd[3] = svF;       // 是否存储标志，false为不存储，true为存储
    cmd[4] = ctrl_mode; // 控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
    cmd[5] = 0x6B;      // 校验字节

    // 发送命令
    emm_transmit(cmd, 6);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    使能信号控制
 * @param    addr  ：电机地址
 * @param    state ：使能状态     ，true为使能电机，false为关闭电机
 * @param    snF   ：多机同步标志 ，false为不启用，true为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_En_Control(uint8_t addr, bool state, bool snF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0xF3;           // 功能码
    cmd[2] = 0xAB;           // 辅助码
    cmd[3] = (uint8_t)state; // 使能状态
    cmd[4] = snF;            // 多机同步运动标志
    cmd[5] = 0x6B;           // 校验字节

    // 发送命令
    emm_transmit(cmd, 6);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    速度模式
 * @param    addr：电机地址
 * @param    dir ：方向       ，0为CW，其余值为CCW
 * @param    vel ：速度       ，范围0 - 5000RPM
 * @param    acc ：加速度     ，范围0 - 255，注意：0是直接启动
 * @param    snF ：多机同步标志，false为不启用，true为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;                // 地址
    cmd[1] = 0xF6;                // 功能码
    cmd[2] = dir;                 // 方向
    cmd[3] = (uint8_t)(vel >> 8); // 速度(RPM)高8位字节
    cmd[4] = (uint8_t)(vel >> 0); // 速度(RPM)低8位字节
    cmd[5] = acc;                 // 加速度，注意：0是直接启动
    cmd[6] = snF;                 // 多机同步运动标志
    cmd[7] = 0x6B;                // 校验字节

    // 发送命令
    emm_transmit(cmd, 8);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    位置模式
 * @param    addr：电机地址
 * @param    dir ：方向        ，0为CW，其余值为CCW
 * @param    vel ：速度(RPM)   ，范围0 - 5000RPM
 * @param    acc ：加速度      ，范围0 - 255，注意：0是直接启动
 * @param    clk ：脉冲数      ，范围0- (2^32 - 1)个
 * @param    raF ：相位/绝对标志，false为相对运动，true为绝对值运动
 * @param    snF ：多机同步标志 ，false为不启用，true为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;                 // 地址
    cmd[1] = 0xFD;                 // 功能码
    cmd[2] = dir;                  // 方向
    cmd[3] = (uint8_t)(vel >> 8);  // 速度(RPM)高8位字节
    cmd[4] = (uint8_t)(vel >> 0);  // 速度(RPM)低8位字节
    cmd[5] = acc;                  // 加速度，注意：0是直接启动
    cmd[6] = (uint8_t)(clk >> 24); // 脉冲数(bit24 - bit31)
    cmd[7] = (uint8_t)(clk >> 16); // 脉冲数(bit16 - bit23)
    cmd[8] = (uint8_t)(clk >> 8);  // 脉冲数(bit8  - bit15)
    cmd[9] = (uint8_t)(clk >> 0);  // 脉冲数(bit0  - bit7 )
    cmd[10] = raF;                 // 相位/绝对标志，false为相对运动，true为绝对值运动
    cmd[11] = snF;                 // 多机同步运动标志，false为不启用，true为启用
    cmd[12] = 0x6B;                // 校验字节

    // 发送命令
    emm_transmit(cmd, 13);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    立即停止（所有控制模式都通用）
 * @param    addr  ：电机地址
 * @param    snF   ：多机同步标志，false为不启用，true为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Stop_Now(uint8_t addr, bool snF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFE; // 功能码
    cmd[2] = 0x98; // 辅助码
    cmd[3] = snF;  // 多机同步运动标志
    cmd[4] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 5);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    多机同步运动
 * @param    addr  ：电机地址
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Synchronous_motion(uint8_t addr)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFF; // 功能码
    cmd[2] = 0x66; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 4);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    设置单圈回零的零点位置
 * @param    addr  ：电机地址
 * @param    svF   ：是否存储标志，false为不存储，true为存储
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Set_O(uint8_t addr, bool svF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x93; // 功能码
    cmd[2] = 0x88; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 5);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    修改回零参数
 * @param    addr  ：电机地址
 * @param    svF   ：是否存储标志，false为不存储，true为存储
 * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * @param    o_dir  ：回零方向，0为CW，其余值为CCW
 * @param    o_vel  ：回零速度，单位：RPM（转/分钟）
 * @param    o_tm   ：回零超时时间，单位：毫秒
 * @param    sl_vel ：无限位碰撞回零检测转速，单位：RPM（转/分钟）
 * @param    sl_ma  ：无限位碰撞回零检测电流，单位：Ma（毫安）
 * @param    sl_ms  ：无限位碰撞回零检测时间，单位：Ms（毫秒）
 * @param    potF   ：上电自动触发回零，false为不使能，true为使能
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF)
{
    uint8_t cmd[32] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;                    // 地址
    cmd[1] = 0x4C;                    // 功能码
    cmd[2] = 0xAE;                    // 辅助码
    cmd[3] = svF;                     // 是否存储标志，false为不存储，true为存储
    cmd[4] = o_mode;                  // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[5] = o_dir;                   // 回零方向
    cmd[6] = (uint8_t)(o_vel >> 8);   // 回零速度(RPM)高8位字节
    cmd[7] = (uint8_t)(o_vel >> 0);   // 回零速度(RPM)低8位字节
    cmd[8] = (uint8_t)(o_tm >> 24);   // 回零超时时间(bit24 - bit31)
    cmd[9] = (uint8_t)(o_tm >> 16);   // 回零超时时间(bit16 - bit23)
    cmd[10] = (uint8_t)(o_tm >> 8);   // 回零超时时间(bit8  - bit15)
    cmd[11] = (uint8_t)(o_tm >> 0);   // 回零超时时间(bit0  - bit7 )
    cmd[12] = (uint8_t)(sl_vel >> 8); // 无限位碰撞回零检测转速(RPM)高8位字节
    cmd[13] = (uint8_t)(sl_vel >> 0); // 无限位碰撞回零检测转速(RPM)低8位字节
    cmd[14] = (uint8_t)(sl_ma >> 8);  // 无限位碰撞回零检测电流(Ma)高8位字节
    cmd[15] = (uint8_t)(sl_ma >> 0);  // 无限位碰撞回零检测电流(Ma)低8位字节
    cmd[16] = (uint8_t)(sl_ms >> 8);  // 无限位碰撞回零检测时间(Ms)高8位字节
    cmd[17] = (uint8_t)(sl_ms >> 0);  // 无限位碰撞回零检测时间(Ms)低8位字节
    cmd[18] = potF;                   // 上电自动触发回零，false为不使能，true为使能
    cmd[19] = 0x6B;                   // 校验字节

    // 发送命令
    emm_transmit(cmd, 20);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    触发回零
 * @param    addr   ：电机地址
 * @param    o_mode ：回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * @param    snF   ：多机同步标志，false为不启用，true为启用
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr;   // 地址
    cmd[1] = 0x9A;   // 功能码
    cmd[2] = o_mode; // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[3] = snF;    // 多机同步运动标志，false为不启用，true为启用
    cmd[4] = 0x6B;   // 校验字节

    // 发送命令
    emm_transmit(cmd, 5);
    emm_wait_for_ack(cmd[1]);
}

/**
 * @brief    强制中断并退出回零
 * @param    addr  ：电机地址
 * @retval   地址 + 功能码 + 命令状态 + 校验字节
 */
void Emm_V5_Origin_Interrupt(uint8_t addr)
{
    uint8_t cmd[16] = {0};
    rt_mutex_take(mutex_step, RT_WAITING_FOREVER);

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x9C; // 功能码
    cmd[2] = 0x48; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    emm_transmit(cmd, 4);
    emm_wait_for_ack(cmd[1]);
}
rt_err_t emm_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    Emm_rx_size = size;
    rt_sem_release(emm_rx_sem);
    return RT_EOK;
}
void drv_emm_v5_entry(void *t)
{
    /* 查找系统中的串口设备 */
    Emm_serial1 = rt_device_find("uart8");
	if(Emm_serial1==RT_NULL)return ;
    char ch = 0;
    /* 初始化信号量 */
    emm_rx_sem = rt_sem_create("emm_rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以阻塞接收及轮询发送模式打开串口设备 */
    rt_device_open(Emm_serial1, RT_DEVICE_FLAG_RX_NON_BLOCKING | RT_DEVICE_FLAG_TX_BLOCKING);
    if (rt_device_set_rx_indicate(Emm_serial1, emm_uart_rx_ind) != RT_EOK)
    {
        LOG_E("uart1 set rx indicate failed");
        return ;
    }


    while (1)
    {
        Emm_V5_Vel_Control(0, 0, 1000, 0, 0);
        rt_thread_mdelay(500);
          Emm_V5_Read_Sys_Params(0, S_VEL);
        rt_thread_mdelay(500);

        //Emm_V5_Stop_Now(0,0);
    }
}
int emm_v5_init(void)
{

    /*互斥锁*/
    mutex_step = rt_mutex_create("mutex_emm", RT_IPC_FLAG_PRIO);
    if (mutex_step == RT_NULL)
        return -RT_ERROR;
    rt_thread_t drv_emm_v5_tb = RT_NULL;

    drv_emm_v5_tb = rt_thread_create("drv_emm_v5",
                                     drv_emm_v5_entry, RT_NULL,
                                     4096,
                                     15, 1);

    if (drv_emm_v5_tb != RT_NULL)
    {
        rt_thread_startup(drv_emm_v5_tb);
    }
    return 0;
}
INIT_APP_EXPORT(emm_v5_init);