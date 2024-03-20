/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-12-21     ChuShicheng  first version
 */

#include <Arduino.h>
//20SFFACTORY COMMUNITY ROBOT FIRMWARE

//MAINTAINER: LEOYEUNG@20SFFACTORY
//CONTACT: yeung.kl.leo@gmail.com
//FORUM: www.facebook.com/groups/robotarm

//VERSION: V0.81

//VERSION HISTORY:
//   V0.31 WITH G92, M114, LOGGER, LIMIT_CHECK FUNCTIONS
//   V0.41 WITH DUAL SHANK LENGTH SUPPORT
//   V0.51 WITH SERVO GRIPPER
//   V0.61 WITH ARDUINO UNO OPTION
//   V0.71 WITH:
//       ESP32(WEMOS D1R32) WITH PS4 JOYSTICK CONTROL OPTION
//       COMMAND TO SET CUSTOM SPEED PROFILE 'M205 S0'
//       UNO OPTION WITH RAIL SUPPORT
//   V0.81 WITH WII REMOTE, JOYSTICK ADJUSTABLE SPEED MULTIPLIER


#include <Arduino.h>
//GENERAL CONFIG SETTINGS
#include "config.h"

#include "robotGeometry.h"
#include "interpolation.h"
//#include "RampsStepper.h"
//#include "command.h"
//#include "equipment.h"
//#include "endstop.h"
#include "logger.h"
#include "Emm_v5.h"
#include "drv_stepper_motor.h"

//EXECUTION & COMMAND OBJECTS 执行和命令对象
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;


float ymm_absolute = 0;
float zmm_absolute = 0;
uint8_t i = 0;


extern "C"
{
  void big_arm_get_pulse(uint32_t pulse);
}

uint32_t big_arm_pulse;
uint32_t small_arm_pulse;

uint8_t big_arm_dir;
uint8_t small_arm_dir;

void setup()
{
  //Serial.begin(BAUD);
    
  // stepperHigher.setPositionRad(PI / 2.0); // 90°
  // stepperLower.setPositionRad(0);         // 0°
  // stepperRotate.setPositionRad(0);        // 0°
  #if RAIL
  stepperRail.setPosition(0);
  #endif
  if (HOME_ON_BOOT) { //HOME DURING SETUP() IF HOME_ON_BOOT ENABLED  如果HOME_ON_BOOT已启用
    // homeSequence(); 
    Logger::logINFO("ROBOT ONLINE");
  } else {
    // setStepperEnable(false); //ROBOT ADJUSTABLE BY HAND AFTER TURNING ON  //机器人开启后可手动调节
    if (HOME_X_STEPPER && HOME_Y_STEPPER && !HOME_Z_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("ROTATE ROBOT TO FACE FRONT CENTRE & SEND G28 TO CALIBRATE");
    }
    if (HOME_X_STEPPER && HOME_Y_STEPPER && HOME_Z_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("SEND G28 TO CALIBRATE");
    }
    if (!HOME_X_STEPPER && !HOME_Y_STEPPER){
      Logger::logINFO("ROBOT ONLINE");
      Logger::logINFO("HOME ROBOT MANUALLY & SEND G28 TO CALIBRATE");
    }
  }

  LOG_D("arduino_setup\n"); 
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
  
interpolator.setInterpolation(interpolator.getXPosmm()+0, interpolator.getYPosmm()+0, interpolator.getZPosmm()+20, interpolator.getEPosmm()+20, 5);


  interpolator.speed_profile = 0;
  ymm_absolute = 50;
  zmm_absolute = 50;

  interpolator.setInterpolation(interpolator.getXPosmm()+0, ymm_absolute-interpolator.getYPosmm(), zmm_absolute-interpolator.getZPosmm(), interpolator.getEPosmm()+0, 5);//这个函数赋的是差值 目标值和当前值之间的距离，插值每次插5mm（细分）
}

void loop() 
{
/*
注意c++的写法，每个电机继承了 RobotGeometry 类，所以可以直接调用 RobotGeometry 类的函数
相当于我们要自己写一个类继承 RobotGeometry 类，然后在这个类里面写我们的电机控制函数
*/
  interpolator.speed_profile = 0;
   //

  ///////////////////////////////插值控制器得运算并且得出结果////////////////////////////////////////////////
  interpolator.updateActualPosition();
  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());//笛卡尔坐标系，把线性插值的每一次小步，更新到笛卡尔坐标系的坐标移动

  // stepperRotate.stepToPositionRad(geometry.getRotRad());//坐标点 转为 角度变量 再转为电机脉冲数
  // stepperLower.stepToPositionRad(geometry.getLowRad());
  // stepperHigher.stepToPositionRad(geometry.getHighRad());
  #if RAIL
    stepperRail.stepToPositionMM(interpolator.getEPosmm(), STEPS_PE11R_MM_RAIL);
  #endif
//////////////////////////////////////////////////////////////////////////////////////////////


 	//LOG_D("M1:%f M3:%f",geometry.getLowRad()*180/3.14, geometry.getHighRad()*180/3.14);
 
  //设目标值
  // if (i<2) {
  // zmm_absolute = zmm_absolute;
  // interpolator.setInterpolation(interpolator.getXPosmm()+0, ymm_absolute-interpolator.getYPosmm(), zmm_absolute-interpolator.getZPosmm(), interpolator.getEPosmm()+0, 5);//这个函数赋的是差值 目标值和当前值之间的距离
  // i++;
  // }

  ///////////////////////////////插值控制器得运算并且得出结果////////////////////////////////////////////////
  //interpolator.updateActualPosition();
  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());//笛卡尔坐标系，把线性插值的每一次小步，更新到笛卡尔坐标系的坐标移动

  //打印当前角度信息
	//LOG_D("arduino:::::::M1:%f M3:%f",geometry.getLowRad()*180/3.14, geometry.getHighRad()*180/3.14); 

  //电机加载
  if(geometry.getLowRad()>0)
  {
    big_arm_dir = 0;
  }
  else
  {
    big_arm_dir = 1;
  }

  if(geometry.getHighRad()>0)
  {
    small_arm_dir = 0;
  }
  else
  {
    small_arm_dir = 1;
  }

  big_arm_pulse = (uint32_t)geometry.getLowRad()*3200/(6.28);
  small_arm_pulse = (uint32_t)geometry.getHighRad()*3200/(6.28);
  //LOG_D("arduino_run\n"); 
  // Emm_V5_Pos_Control(1, big_arm_dir, 100, 20, big_arm_pulse, false, false);
  // Emm_V5_Pos_Control(3, small_arm_dir, 100, 20, small_arm_pulse, false, false);

  big_arm_get_pulse(big_arm_pulse);

  delay(500);

//  Emm_V5_Pos_Control(1, 0, 100, 20, geometry.getLowRad()*3200/(6.28), false, false);
//   Emm_V5_Pos_Control(3, 0, 100, 20, geometry.getHighRad()*3200/(6.28), false, false);

//geometry.getRotRad()*3200/(6.28)

}
