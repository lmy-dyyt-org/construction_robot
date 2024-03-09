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
#include "RampsStepper.h"
#include "command.h"
#include "equipment.h"
#include "endstop.h"
#include "logger.h"
#include "Emm_v5.h"

//EXECUTION & COMMAND OBJECTS
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;


void setup()
{
  //Serial.begin(BAUD);

  // stepperHigher.setPositionRad(PI / 2.0); // 90°
  // stepperLower.setPositionRad(0);         // 0°
  // stepperRotate.setPositionRad(0);        // 0°
  #if RAIL
  stepperRail.setPosition(0);
  #endif
  if (HOME_ON_BOOT) { //HOME DURING SETUP() IF HOME_ON_BOOT ENABLED
    // homeSequence(); 
    Logger::logINFO("ROBOT ONLINE");
  } else {
    // setStepperEnable(false); //ROBOT ADJUSTABLE BY HAND AFTER TURNING ON
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
  interpolator.setInterpolation(INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0, INITIAL_X, INITIAL_Y, INITIAL_Z, INITIAL_E0);
}

void loop() {
/*
注意c++的写法，每个电机继承了 RobotGeometry 类，所以可以直接调用 RobotGeometry 类的函数
相当于我们要自己写一个类继承 RobotGeometry 类，然后在这个类里面写我们的电机控制函数

*/
  ///////////////////////////////插值控制器得运算并且得出结果////////////////////////////////////////////////
  interpolator.updateActualPosition();
  geometry.set(interpolator.getXPosmm(), interpolator.getYPosmm(), interpolator.getZPosmm());
  // stepperRotate.stepToPositionRad(geometry.getRotRad());
  // stepperLower.stepToPositionRad(geometry.getLowRad());
  // stepperHigher.stepToPositionRad(geometry.getHighRad());
  #if RAIL
    stepperRail.stepToPositionMM(interpolator.getEPosmm(), STEPS_PER_MM_RAIL);
  #endif
//////////////////////////////////////////////////////////////////////////////////////////////

  // Emm_V5_Pos_Control(1, 0, 100, 0, geometry.getLowRad()*3200/(6.28), false, false);
  // Emm_V5_Pos_Control(1, 0, 100, 0, geometry.getHighRad()*3200/(6.28), false, false);


  //while(电机运动到位置)
  delay(10);


//geometry.getRotRad()*3200/(6.28)




}
