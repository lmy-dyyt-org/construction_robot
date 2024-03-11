#include "follow_line.h"

infrared infrared_package;

void follow_line(void *parameter)
{
	Infrared_Init();
	infrared_package.move_direction = front;
  while(1)
  {
    /* 线程处理 */
	GET_Infrared_Data(&infrared_package);
		/* 线程运行，打印计数 */
    Print_Infrared_Data(&infrared_package);
	rt_kprintf("is_spacial_point_flag:%d\n",Is_Spacial_point(&infrared_package));
    rt_thread_mdelay(500);
  }
}

//以上帝视角为准,引入 拿/放（反转视角）时,小车行进状态:直行、左行、右行
uint8_t Is_Spacial_point(infrared* infrared_package)
{
	switch(infrared_package->move_direction)
	{
		case front: 
		if(infrared_package->infrared_data[front_left0_infrared] || infrared_package->infrared_data[front_right1_infrared])
			infrared_package->is_spacial_point_flag = 1;
		else infrared_package->is_spacial_point_flag = 0;
		break;

		case left:
		if(infrared_package->infrared_data[left_left0_infrared] || infrared_package->infrared_data[left_right1_infrared])
			infrared_package->is_spacial_point_flag = 1;
		else infrared_package->is_spacial_point_flag = 0;
		break;

		case right:
		if(infrared_package->infrared_data[right_left0_infrared] || infrared_package->infrared_data[right_right1_infrared])
			infrared_package->is_spacial_point_flag = 1;
		else infrared_package->is_spacial_point_flag = 0;
		break;

		default: infrared_package->is_spacial_point_flag = 0;
		break;
	}
	return infrared_package->is_spacial_point_flag;
}

void Infrared_Init(void)
{
	rt_pin_mode(FRONT_INFRARED0_PIN, PIN_MODE_INPUT);
	rt_pin_mode(FRONT_INFRARED1_PIN, PIN_MODE_INPUT);
	rt_pin_mode(FRONT_INFRARED2_PIN, PIN_MODE_INPUT);
	rt_pin_mode(FRONT_INFRARED3_PIN, PIN_MODE_INPUT);
	rt_pin_mode(FRONT_INFRARED4_PIN, PIN_MODE_INPUT);
	rt_pin_mode(FRONT_INFRARED5_PIN, PIN_MODE_INPUT);

	rt_pin_mode(LEFT_INFRARED0_PIN, PIN_MODE_INPUT);
	rt_pin_mode(LEFT_INFRARED1_PIN, PIN_MODE_INPUT);
	rt_pin_mode(LEFT_INFRARED2_PIN, PIN_MODE_INPUT);
	rt_pin_mode(LEFT_INFRARED3_PIN, PIN_MODE_INPUT);
	rt_pin_mode(LEFT_INFRARED4_PIN, PIN_MODE_INPUT);
	rt_pin_mode(LEFT_INFRARED5_PIN, PIN_MODE_INPUT);

	rt_pin_mode(RIGHT_INFRARED0_PIN, PIN_MODE_INPUT);
	rt_pin_mode(RIGHT_INFRARED1_PIN, PIN_MODE_INPUT);
	rt_pin_mode(RIGHT_INFRARED2_PIN, PIN_MODE_INPUT);
	rt_pin_mode(RIGHT_INFRARED3_PIN, PIN_MODE_INPUT);
	rt_pin_mode(RIGHT_INFRARED4_PIN, PIN_MODE_INPUT);
	rt_pin_mode(RIGHT_INFRARED5_PIN, PIN_MODE_INPUT);
}

void GET_Infrared_Data(infrared* infrared_package)
{
	infrared_package->infrared_data[front_left0_infrared] = rt_pin_read(FRONT_INFRARED0_PIN);
	infrared_package->infrared_data[front_left1_infrared] = rt_pin_read(FRONT_INFRARED1_PIN);
	infrared_package->infrared_data[front_middle0_infrared] = rt_pin_read(FRONT_INFRARED2_PIN);
	infrared_package->infrared_data[front_middle1_infrared] = rt_pin_read(FRONT_INFRARED3_PIN);
	infrared_package->infrared_data[front_right0_infrared] = rt_pin_read(FRONT_INFRARED4_PIN);
	infrared_package->infrared_data[front_right1_infrared] = rt_pin_read(FRONT_INFRARED5_PIN);

	infrared_package->infrared_data[left_left0_infrared] = rt_pin_read(LEFT_INFRARED0_PIN);
	infrared_package->infrared_data[left_left1_infrared] = rt_pin_read(LEFT_INFRARED1_PIN);
	infrared_package->infrared_data[left_middle0_infrared] = rt_pin_read(LEFT_INFRARED2_PIN);
	infrared_package->infrared_data[left_middle1_infrared] = rt_pin_read(LEFT_INFRARED3_PIN);
	infrared_package->infrared_data[left_right0_infrared] = rt_pin_read(LEFT_INFRARED4_PIN);
	infrared_package->infrared_data[left_right1_infrared] = rt_pin_read(LEFT_INFRARED5_PIN);

	infrared_package->infrared_data[right_left0_infrared] = rt_pin_read(RIGHT_INFRARED0_PIN);
	infrared_package->infrared_data[right_left1_infrared] = rt_pin_read(RIGHT_INFRARED1_PIN);
	infrared_package->infrared_data[right_middle0_infrared] = rt_pin_read(RIGHT_INFRARED2_PIN);
	infrared_package->infrared_data[right_middle1_infrared] = rt_pin_read(RIGHT_INFRARED3_PIN);
	infrared_package->infrared_data[right_right0_infrared] = rt_pin_read(RIGHT_INFRARED4_PIN);
	infrared_package->infrared_data[right_right1_infrared] = rt_pin_read(RIGHT_INFRARED5_PIN);
}

void Print_Infrared_Data(infrared* infrared_package)
{
	rt_kprintf("infrared_package_data:\n");
	rt_kprintf("          "); 

	rt_kprintf("%d %d %d %d %d %d\n", infrared_package->infrared_data[front_left0_infrared]
									, infrared_package->infrared_data[front_left1_infrared]
									, infrared_package->infrared_data[front_middle0_infrared]
									, infrared_package->infrared_data[front_middle1_infrared]
									, infrared_package->infrared_data[front_right0_infrared]
									, infrared_package->infrared_data[front_right1_infrared]);

	rt_kprintf("%d %d %d %d %d %d"  , infrared_package->infrared_data[left_left0_infrared]
									, infrared_package->infrared_data[left_left1_infrared]
									, infrared_package->infrared_data[left_middle0_infrared]
									, infrared_package->infrared_data[left_middle1_infrared]
									, infrared_package->infrared_data[left_right0_infrared]
									, infrared_package->infrared_data[left_right1_infrared]);

	rt_kprintf("          "); 

	rt_kprintf("%d %d %d %d %d %d\n", infrared_package->infrared_data[right_left0_infrared]
									, infrared_package->infrared_data[right_left1_infrared]
									, infrared_package->infrared_data[right_middle0_infrared]
									, infrared_package->infrared_data[right_middle1_infrared]
									, infrared_package->infrared_data[right_right0_infrared]
									, infrared_package->infrared_data[right_right1_infrared]);

	rt_kprintf("\n");
}


