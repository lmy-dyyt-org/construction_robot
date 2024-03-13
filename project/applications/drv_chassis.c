#include "drv_chassis.h"
#include "motor.h"

chassis_mai_t mai;
chassis_mai_state state;
chassis_mai_offset offset;

void drv_chassis(void *parameter)
{
  chassis_mai_init(&mai,chassis_mai_userdata);
  state.xSpeed_m_s = 5;
  while(1)
  {
    /* 线程处理 */
    chassis_mai_set_speed(&mai, &state, &offset);
	/* 线程运行，打印计数 */
    
    rt_thread_mdelay(500);
  }
}

void chassis_mai_userdata(chassis_mai_t * mai)
{

}

/**
 * @brief 将给定xy速度分解到，  ab轴上（x型）【/：a方向  \：b方向】 ， ac轮对b方向起作用，bd轮对a方向起作用
 *        所以给定整车的vx 和 vy，通过xy轴和ab轴之间的关系，就可设定va 和 vb
 *        电机只能是y轴转动，那么也需要分解到对应的a轴或b轴。  知道va或vb就可以逆推每个轮子的转速
 */
///////////////////////////////////////////////////////用户输入m/s  但是对于加载电机的转速 就要变为rmp
void chassis_mai_set_speed(chassis_mai_t * mai, chassis_mai_state* state, chassis_mai_offset* offset)
{
    #define tmp0 ((CSS_MAI_TYPE)6.7523723711782955216639939325505543699842)
    CSS_MAI_TYPE wheel_0,wheel_1,wheel_2,wheel_3;//定义车轮0-3 速度 m/s
    mai->state.wSpeed_rad_s = state->wSpeed_rad_s;
    mai->state.xSpeed_m_s = state->xSpeed_m_s;
    mai->state.ySpeed_m_s = state->ySpeed_m_s;

    //以下注释代码方便理解具体实现
    //Va=speed_x*cos(45)+speed_y*sin(45); // 计算a方向的速度
    //Vb=speed_x*sin(45)-speed_y*cos(45); // 计算b方向的速度

    //   //wheel_x_rmp_mins 车轮转速每分钟
    // 由方程 wheel_speedx = (wheel_x_rmp_mins* 2*pi*mai->wheel_r_m)/60
    // 得到 wheel_x_rmp_mins = 60*wheel_speedx/(2*pi*mai->wheel_r_m)
    //wheel_x_rmp_mins = 9.54929658551372014*wheel_speedx/mai->wheel_r_m;
    //而wheel_speedx = (xSpeed_m_s + ySpeed_m_s)*1.414/2

    //所以由下面的简化算式

    //自转速度使用公式简化推导过程
    //其中R为车辆几何中心到车轮距离 a为车辆边长
    //v为车轮线速度w为车辆自转速度，单位为弧度每秒
    //rmp为车轮转速rmp/min r为车轮半径
    // R = 根号(a2 + b2 ) / 1.14
    // v=wR
    // v=(rmp/60)*(2*pi*r)
    // wa=(rmp)*(2*pi*r)*1.14/60
    //得到rmp = w*a/(0.14809609793861220823386269966868*r)
    //临时变量,加速运算(可能吧)
    CSS_MAI_TYPE tmp1 = (state->xSpeed_m_s + state->ySpeed_m_s)*tmp0/mai->wheel_r_m;
    CSS_MAI_TYPE tmp2 = (state->xSpeed_m_s - state->ySpeed_m_s)*tmp0/mai->wheel_r_m;
    CSS_MAI_TYPE tmp3 = state->wSpeed_rad_s*sqrt(mai->chssis_a_m * mai->chssis_a_m + mai->chssis_b_m * mai->chssis_b_m)*tmp0/mai->wheel_r_m;
    
    mai->wheel_0_speed_m_s =  tmp1 + tmp3;
    mai->wheel_1_speed_m_s =  tmp2 - tmp3;
    mai->wheel_2_speed_m_s =  tmp1 - tmp3;
    mai->wheel_3_speed_m_s =  tmp2 + tmp3;

    //引入偏差量,保证车辆正常达到目标速度
    mai->wheel_0_speed_m_s += offset->wheel_0_offset_speed_m_s;
    mai->wheel_1_speed_m_s += offset->wheel_1_offset_speed_m_s;
    mai->wheel_2_speed_m_s += offset->wheel_2_offset_speed_m_s;
    mai->wheel_3_speed_m_s += offset->wheel_3_offset_speed_m_s;
		
	//			motor_set_speed(M2006_1_CAN1,-mai->wheel_0_speed_m_s);
//				motor_set_speed(M2006_2_CAN1,-mai->wheel_1_speed_m_s);
				motor_set_speed(M2006_3_CAN1,mai->wheel_2_speed_m_s);
//				motor_set_speed(M2006_4_CAN1,-mai->wheel_3_speed_m_s);
}

//备注:航向角是以开机为0角度，由于没有添加其他纠正手段故仅在车轮无打滑，地面平行的时候才能保证精度（保证不了！！）
void chassis_mai_set_position(chassis_mai_t * mai,chassis_mai_state* state, chassis_mai_offset* offset)
{
    #define tmp0_2 ((CSS_MAI_TYPE)1.4142135623730950488016887242097)
    #define tmp0_1 ((CSS_MAI_TYPE)0.70710678118654752440084436210485)
    mai->state.x_m = state->x_m;
    mai->state.y_m = state->y_m;
    mai->state.yaw_rad = state->yaw_rad;

    //角度同速度相同推导，不再赘述
    //简要推导
    //(pos_rad/(2*pi))*2*pi*r = wheel_pos
    //pos_rad = wheel_pos/r

    //l=w_pos*R
    //l=w_wheel_pos*r
    //w_pos*R = w_wheel_pos*r

    CSS_MAI_TYPE tmp1 = (state->x_m + state->y_m)*tmp0_2/mai->wheel_r_m;
    CSS_MAI_TYPE tmp2 = (state->x_m - state->y_m)*tmp0_2/mai->wheel_r_m;
    CSS_MAI_TYPE tmp3 = state->yaw_rad*sqrt(mai->chssis_a_m * mai->chssis_a_m + mai->chssis_b_m * mai->chssis_b_m)*tmp0_1/mai->wheel_r_m;

    mai->wheel_0_pos_rad += (tmp1 + tmp3);
    mai->wheel_1_pos_rad += (tmp2 - tmp3);
    mai->wheel_2_pos_rad += (tmp1 - tmp3);
    mai->wheel_3_pos_rad += (tmp2 + tmp3);

    //引入偏差量,保证车辆正常达到目标位置
    mai->wheel_0_pos_rad += offset->wheel_0_offset_pos_m;
    mai->wheel_1_pos_rad += offset->wheel_1_offset_pos_m;
    mai->wheel_2_pos_rad += offset->wheel_2_offset_pos_m;
    mai->wheel_3_pos_rad += offset->wheel_3_offset_pos_m;
}

void chassis_mai_set_zero_pos(chassis_mai_t * mai)
{
   mai->wheel_0_pos_rad = 0;
   mai->wheel_1_pos_rad = 0;
   mai->wheel_2_pos_rad = 0;
   mai->wheel_3_pos_rad = 0;
}

//传入间隔事件 单位ms
void chassis_mai_handler(chassis_mai_t * mai,uint32_t time_ms)
{
    if(mai->user_handle)
    mai->user_handle(mai);
}

#define chassis_mai_get_speed chassis_mai_get_state
#define chassis_mai_get_position chassis_mai_get_state

void chassis_mai_get_state(chassis_mai_t * mai, chassis_mai_state* state)
{
    memcpy(state,&mai->state,sizeof(chassis_mai_state));
}

int chassis_mai_init(chassis_mai_t * mai,void(*user_handle)(chassis_mai_t* mai))
{
    if(mai){
        memset(mai,0,sizeof(chassis_mai_t));
        mai->user_handle = user_handle;

        mai->wheel_r_m = WHELL_R_M;//车轮子半径单位m
        mai->chssis_a_m = CHSSIS_A_M;//车x方向直径单位m
        mai->chssis_b_m = CHSSIS_B_M;//车y方向直径单位m

        return 0;
    }else{
        return-1;
    }
}


























