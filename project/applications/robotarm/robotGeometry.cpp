#include "robotGeometry.h"

#include <math.h>
#include <Arduino.h>
/*
核心文件，用于计算机械臂的几何学，包括计算机械臂的角度，长度等
RobotGeometry 类的构造函数接收三个参数：a_ee_offset（末端执行器偏移）、
a_low_shank_length（下杆长度）和 a_high_shank_length（上杆长度）这些参数用于设置机器人的几何形状

set 方法接收三个参数：axmm、aymm 和 azmm，
分别代表机器人在三个方向（x、y、z）上的位置
在设置这些位置后，会调用 calculateGrad 方法来计算一些必要的参数。

getXmm、getYm    m 和 getZmm 方法分别用于获取机器人在三个方向上的位置。

注意！！
我们要了解这个文件的作用，可以尝试理解这个文件的代码，注意我们机器人没有z轴的运动，rotation
*/
RobotGeometry::RobotGeometry(float a_ee_offset, float a_low_shank_length, float a_high_shank_length) {
  ee_offset = a_ee_offset;
  low_shank_length = a_low_shank_length;
  high_shank_length = a_high_shank_length;
}

void RobotGeometry::set(float axmm, float aymm, float azmm) {
  xmm = axmm;
  ymm = aymm;
  zmm = azmm; 
  calculateGrad();
}

float RobotGeometry::getXmm() const {
  return xmm;
}

float RobotGeometry::getYmm() const {
  return ymm;
}

float RobotGeometry::getZmm() const {
  return zmm;
}

float RobotGeometry::getRotRad() const {
  return rot;
}

float RobotGeometry::getLowRad() const {
  return low;
}

float RobotGeometry::getHighRad() const {
  return high;
}

float RobotGeometry::getHypot() const {
  return rrot_ee;
}

void RobotGeometry::calculateGrad() {
   rrot_ee =  hypot(xmm, ymm);    
   float rrot = rrot_ee - ee_offset; //radius from Top View
   float rside = hypot(rrot, zmm);  //radius from Side View. Use rrot instead of ymm..for everything
   float rside_2 = sq(rside);
   float low_2 = sq(low_shank_length);
   float high_2 = sq(high_shank_length);
   
   rot = asin(xmm / rrot_ee);
   high = PI - acos((low_2 + high_2 - rside_2) / (2 * low_shank_length * high_shank_length));

   //Angle of Lower Stepper Motor  (asin()=Angle To Gripper)
   if (zmm > 0) {
     low =  acos(zmm / rside) - acos((low_2 - high_2 + rside_2) / (2 * low_shank_length * rside));
   } else {
     low = PI - asin(rrot / rside) - acos((low_2 - high_2 + rside_2) / (2 * low_shank_length * rside));
   }
   high = high + low;
}
