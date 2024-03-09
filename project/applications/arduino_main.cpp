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
#include "queue.h"
#include "command.h"
#include "equipment.h"
#include "endstop.h"
#include "logger.h"
//INCLUDE CORRESPONDING GRIPPER MOTOR CLASS
// #if GRIPPER == SERVO
//   #include "servo_gripper.h"
// #elif GRIPPER == BYJ
//   #include "byj_gripper.h"
// #endif

//DETERMINE PINOUTS & CONFIG TO USE SUBJECT TO BOARD_CHOICE
// #if BOARD_CHOICE == UNO
//   #include "pinout/pinout_uno.h"
// #elif BOARD_CHOICE == WEMOSD1R32
//   #include "pinout/pinout_wemosD1R32.h"
//   #include "config_esp32.h"
// #elif BOARD_CHOICE == MEGA2560
//   #include "pinout/pinout.h"
// #endif

//STEPPER OBJECTS
// RampsStepper stepperHigher(X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, INVERSE_X_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
// RampsStepper stepperLower(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, INVERSE_Y_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
// RampsStepper stepperRotate(Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, INVERSE_Z_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);

//RAIL OBJECTS
#if RAIL
  RampsStepper stepperRail(E0_STEP_PIN, E0_DIR_PIN, E0_ENABLE_PIN, INVERSE_E0_STEPPER, MAIN_GEAR_TEETH, MOTOR_GEAR_TEETH, MICROSTEPS, STEPS_PER_REV);
  #if BOARD_CHOICE == WEMOSD1R32 //PINSWAP REQIURED ON D1R32 DUE TO INSUFFICIENT DIGIAL PINS
    #define SERVO_PIN 23 // REDEFINE SERVO_PIN FOR RAIL // SHARE WITH Z_MIN_PIN
    Endstop endstopE0(E0_MIN_PIN, E0_DIR_PIN, E0_STEP_PIN, E0_ENABLE_PIN, E0_MIN_INPUT, E0_HOME_STEPS, HOME_DWELL, true);
  #else 
    Endstop endstopE0(E0_MIN_PIN, E0_DIR_PIN, E0_STEP_PIN, E0_ENABLE_PIN, E0_MIN_INPUT, E0_HOME_STEPS, HOME_DWELL, false);
  #endif
#endif

//ENDSTOP OBJECTS
// Endstop endstopX(X_MIN_PIN, X_DIR_PIN, X_STEP_PIN, X_ENABLE_PIN, X_MIN_INPUT, X_HOME_STEPS, HOME_DWELL, false);
// Endstop endstopY(Y_MIN_PIN, Y_DIR_PIN, Y_STEP_PIN, Y_ENABLE_PIN, Y_MIN_INPUT, Y_HOME_STEPS, HOME_DWELL, false);
// #if BOARD_CHOICE == WEMOSD1R32
//   Endstop endstopZ(Z_MIN_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_ENABLE_PIN, Z_MIN_INPUT, Z_HOME_STEPS, HOME_DWELL, true);
// #else
//   Endstop endstopZ(Z_MIN_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_ENABLE_PIN, Z_MIN_INPUT, Z_HOME_STEPS, HOME_DWELL, false);
// #endif

// //EQUIPMENT OBJECTS
// #if GRIPPER == SERVO
//   Servo_Gripper servo_gripper(SERVO_PIN, SERVO_GRIP_DEGREE, SERVO_UNGRIP_DEGREE);
// #elif GRIPPER == BYJ
//   BYJ_Gripper byj_gripper(BYJ_PIN_0, BYJ_PIN_1, BYJ_PIN_2, BYJ_PIN_3, BYJ_GRIP_STEPS);
// #endif
// Equipment laser(LASER_PIN);
// Equipment pump(PUMP_PIN);
// Equipment led(LED_PIN);
// FanControl fan(FAN_PIN, FAN_DELAY);

//EXECUTION & COMMAND OBJECTS
RobotGeometry geometry(END_EFFECTOR_OFFSET, LOW_SHANK_LENGTH, HIGH_SHANK_LENGTH);
Interpolation interpolator;
// Queue<Cmd> queue(QUEUE_SIZE);
// Command command;

//PS4 CONTROLLER OBJECT FOR ESP32
// #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == DUALSHOCK4
//   #include "controller_ps4.h"
//   Controller_PS4 controller_ps4(PS4_MAC);
// #endif
// #if BOARD_CHOICE == WEMOSD1R32 && ESP32_JOYSTICK == WIIMOTE
//   #include "controller_wiimote.h"
//   Controller_Wiimote controller_wiimote;
// #endif

void setup()
{
  Serial.begin(BAUD);

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



  ////////////步进电机控制片，我们要换成自己的闭环步进电机/////////////////
  // stepperRotate.update();
  // stepperLower.update();
  // stepperHigher.update();
  // #if RAIL
  //   stepperRail.update();
  // #endif
//////////////////////////////////////


}
