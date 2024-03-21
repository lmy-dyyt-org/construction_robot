#include <Arduino.h>
#include "RampsStepper.h"

/*
它是用于控制步进电机的，
RampsStepper 类的构造函数接收一些参数，包括步进电机的步进引脚、方向引脚、使能引脚，以及一些与电机相关的参数，
如主齿轮的齿数、电机齿轮的齿数、微步数和每转步数。这些参数用于设置电机的减速比和步进位置

类中的 enable 方法用于启用或禁用步进电机
isOnPosition 方法用于检查步进电机是否已经到达目标位置
getPosition 方法用于获取当前步进电机的位置

注意！！！！！！！！！！！
这个文件我们要实现类似功能替换这个文件，因为这个文件是用于控制步进电机的，我们的项目中使用步进电机步进电机
*/
RampsStepper::RampsStepper(int aStepPin, int aDirPin, int aEnablePin, bool aInverse, float main_gear_teeth, float motor_gear_teeth, int microsteps, int steps_per_rev) {
  setReductionRatio(main_gear_teeth / motor_gear_teeth, microsteps * steps_per_rev);
  stepPin = aStepPin;
  dirPin = aDirPin;
  enablePin = aEnablePin;
  inverse = aInverse;
  stepperStepPosition = 0;
//  stepperStepTargetPosition;
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  enable(false);
}

void RampsStepper::enable(bool value) {
  digitalWrite(enablePin, !value);
}

bool RampsStepper::isOnPosition() const {
  return stepperStepPosition == stepperStepTargetPosition;
}

int RampsStepper::getPosition() const {
  return stepperStepPosition;
}

void RampsStepper::setPosition(int value) {
  stepperStepPosition = value;
  stepperStepTargetPosition = value;
}

void RampsStepper::stepToPosition(int value) {
  stepperStepTargetPosition = value;
}

void RampsStepper::stepToPositionMM(float mm, float steps_per_mm) {
  stepperStepTargetPosition = mm * steps_per_mm;
}

void RampsStepper::stepRelative(int value) {
  value += stepperStepPosition;
  stepToPosition(value);
}

float RampsStepper::getPositionRad() const {
  return stepperStepPosition / radToStepFactor;
}

void RampsStepper::setPositionRad(float rad) {
  setPosition(rad * radToStepFactor);
}

void RampsStepper::stepToPositionRad(float rad) {
  stepperStepTargetPosition = rad * radToStepFactor;
}

void RampsStepper::stepRelativeRad(float rad) {
  stepRelative(rad * radToStepFactor);
}

void RampsStepper::update() {   
  while (stepperStepTargetPosition < stepperStepPosition) {  
    digitalWrite(dirPin, !inverse);
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    stepperStepPosition--;
  }
  
  while (stepperStepTargetPosition > stepperStepPosition) {    
    digitalWrite(dirPin, inverse);
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    stepperStepPosition++;
  }
}

void RampsStepper::setReductionRatio(float gearRatio, int stepsPerRev) {
  radToStepFactor = gearRatio * stepsPerRev / 2 / PI;
};
