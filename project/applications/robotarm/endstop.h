/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-09 00:39:48
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-09 00:51:49
 * @FilePath: \construction_robot\project\applications\robotarm\endstop.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef ENDSTOP_H_
#define ENDSTOP_H_
#if 0
class Endstop {
  public:
    Endstop(int a_min_pin, int a_dir_pin, int a_step_pin, int a_en_pin, int a_switch_input, int a_step_offset, int a_home_dwell, bool does_swap_pin);
    void home(bool dir);
    void homeOffset(bool dir);
    void oneStepToEndstop(bool dir);
    bool state();
    bool bState;

  private:
    int min_pin;
    int dir_pin;
    int step_pin;
    int en_pin;
    int switch_input;
    int home_dwell;
    int step_offset;
    bool swap_pin;    
};

#endif
#endif
