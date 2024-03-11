#include "follow_line.h"

infrared infrared_package;

void follow_line(void *parameter)
{
	Infrared_Init();
	infrared_package.move_direction = front;

    // aStateMachine* sm = (aStateMachine*)malloc(sizeof(aStateMachine));
    // aStateMachine_Init(sm, "sm");
    // aState* s1 = (aState*)malloc(sizeof(aState));
    // aState_Init(s1, "s1", aState_excution, aState_entry, aState_exit);
    // aState* s2 = (aState*)malloc(sizeof(aState));
    // aState_Init(s2, "s2", aState_excution, aState_entry, aState_exit);
    // aState* s3 = (aState*)malloc(sizeof(aState));
    // aState_Init(s3, "s3", aState_excution, aState_entry, aState_exit);
    // aState* s4 = (aState*)malloc(sizeof(aState));
    // aState_Init(s4, "s4", aState_excution, aState_entry, aState_exit);
    // aStateMachine_addState(sm, s1);
    // aStateMachine_addState(sm, s2);
    // aStateMachine_addState(sm, s3);
    // aStateMachine_addState(sm, s4);

    // aStateMachine_start(sm, s1);
    // aStateMachine_addTransition1(sm, s1, s2, condition); //[](aStateMachine* sm) {printf("%s condition s1 -> s2", aState_getName(sm->current)); return false; }
    // aStateMachine_addTransition1(sm, s1, s3, condition);//[](aStateMachine* sm) {printf("%s condition s1 -> s3", aState_getName(sm->current)); return true; }
    // aStateMachine_addTransition1(sm, s2, s3, condition);//[](aStateMachine* sm) {return true; }
    // aStateMachine_addTransition1(sm, s3, s4, condition);
    // aStateMachine_addTransition1(sm, s4, 0, condition);
    // aStateMachine_showStates(sm);

    // //update �����״̬��״̬����ִ��excution Ȼ���ж�condtion
    // aStateMachine_update(sm);
    // aStateMachine_update(sm);
    // aStateMachine_update(sm);
    // aStateMachine_update(sm);
    // aStateMachine_update(sm);

  while(1)
  {
    /* �̴߳��� */
	GET_Infrared_Data(&infrared_package);
		/* �߳����У���ӡ���� */
    // Print_Infrared_Data(&infrared_package);
	// rt_kprintf("is_spacial_point_flag:%d\n",Is_Spacial_point(&infrared_package));

    rt_thread_mdelay(500);
  }
}

//���ϵ��ӽ�Ϊ׼,���� ��/�ţ���ת�ӽǣ�ʱ,С���н�״̬:ֱ�С����С�����
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

		case rotate:
			infrared_package->is_spacial_point_flag = 0;
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


