/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-13 22:05:59
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-25 22:43:22
 * @FilePath: \construction_robot\project\applications\map\PathFinder.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef PATHFINDER_H
#define PATHFINDER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "stdint.h"
    typedef int Path_table_element_t;

    typedef struct
    {
        int start_point_x;
        int start_point_y;
        Path_table_element_t *table;
        char *name;
        int index;

    } Path_table_t;
    extern Path_table_element_t soduko[];
    extern Path_table_element_t transport[];
    void Path_table_init(Path_table_t *table, Path_table_element_t *table_data, char *name, int start_point_x, int start_point_y);

    uint8_t Path_get_next_dir(Path_table_t *table);

#ifdef __cplusplus
}
#endif
#endif