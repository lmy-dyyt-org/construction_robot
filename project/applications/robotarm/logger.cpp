/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-09 00:39:48
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-04 20:52:52
 * @FilePath: \construction_robot\project\applications\robotarm\logger.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "config.h"
#include "logger.h"
//日志输出，不管它，后面改掉
void Logger::log(String message, int level) {
  if(LOG_LEVEL >= level) {
    String logMsg;
    switch(level) {
      case LOG_ERROR:
        logMsg = "ERROR: ";
      break;
      case LOG_INFO:
        logMsg = "INFO: ";
      break;
      case LOG_DEBUG:
        logMsg = "DEBUG: ";
      break;
    }
    logMsg = logMsg + message;
    //Serial.println(logMsg);
  }
}

void Logger::logERROR(String message) {
  log(message, LOG_ERROR);
}
void Logger::logINFO(String message) {
  log(message, LOG_INFO);
}
void Logger::logDEBUG(String message) {
  log(message, LOG_DEBUG);
}