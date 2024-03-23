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

#define main_gear_teeth 90.0
#define motor_gear_teeth 20.0
#define microsteps 16
#define steps_per_rev 200

//EXECUTION & COMMAND OBJECTS 执行和命令对象
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;

uint8_t i = 0;

extern "C"
{
  int32_t abig_arm_pulse;
  int32_t asmall_arm_pulse;
  float ymm;
  float zmm;
}

int32_t big_arm_TargetPosition;
int32_t small_arm_TargetPosition;

float radToStepFactor;

void setup()
{
  //Serial.begin(BAUD);
  // stepperHigher.setPositionRad(PI / 2.0); // 90° //设置初始位置
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
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0); //设初始位置
  ymm = INITIAL_Y;
  zmm = INITIAL_Z;    
  interpolator.speed_profile = 0;
  radToStepFactor = (main_gear_teeth / motor_gear_teeth) * (microsteps * steps_per_rev) / 2 / PI; //减速比设置
}

void loop() 
{
  //输入目标绝对值！！！！！！！！！注意原点的位置！！！！！！！
  interpolator.setInterpolation(0,ymm,zmm,0,5);// 1.注意原点是底座而不是末端执行器  2.注意用绝对坐标，，同时电机也就需要绝对坐标
  //由插值器计算每一次插值后得位置
  interpolator.updateActualPosition();
  //空间解算得夹角
  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());//笛卡尔坐标系，把线性插值的每一次小步，更新到笛卡尔坐标系的坐标移动
  //计算电机需要的脉冲数
  big_arm_TargetPosition = (int32_t)(geometry.getLowRad() * radToStepFactor );//严格按照别人的公式来算，不要自己瞎想
  small_arm_TargetPosition = (int32_t)(geometry.getHighRad() * radToStepFactor );
  //打印当前角度信息
	// LOG_D("arduino:::angle::::big:%f small:%f",geometry.getLowRad()*180/3.14, geometry.getHighRad()*180/3.14); 
  // LOG_D("arduino::::pulse:::big:%d small:%d",big_arm_TargetPosition, small_arm_TargetPosition-3600); //因为机械臂的初始值是90度，所以每次运算脉冲时，要减去90度的脉冲数
  //逻辑线程丢坐标到这个文件，然后其只用来计算脉冲，然后再丢到电机发送线程里，避免两个线程发送冲突
  abig_arm_pulse = big_arm_TargetPosition;
  asmall_arm_pulse = small_arm_TargetPosition-3600;

  delay(500);
}
