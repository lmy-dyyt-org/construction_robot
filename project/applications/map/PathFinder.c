/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-03-13 22:05:59
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-03-25 22:58:34
 * @FilePath: \construction_robot\project\applications\map\PathFinder.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "PathFinder.h"

/*
        "  x   \r\n",        2
        "x   x \r\n",       3  1      0原地不动
        "  x   \r\n",        4
*/
//小车每次任务起点从1/5开始
/* 任务顺序
    9 4 3
    8 5 2
    7 6 1   
*/

Path_table_element_t soduko_take1[]={
    4,3
};
Path_table_element_t soduko_put1[]={
    2,1,2,0
};//第一次记录颜色位置，缺省最后一次特殊点
Path_table_element_t soduko_back1[]={
    4,3,0
};//缺省第一次特殊点


Path_table_element_t soduko_go2[]={
    0
};
Path_table_element_t soduko_put2[]={
    1,2,0
};
Path_table_element_t soduko_back2[]={
    4,3,0
};

Path_table_element_t soduko_go3[]={
    2,3
};
Path_table_element_t soduko_go4[]={
    2,3,3
};
Path_table_element_t soduko_go5[]={
    2,3,3
};
Path_table_element_t soduko_go6[]={
    4,3,3
};
Path_table_element_t soduko_go7[]={
    4,3,3,3
};
Path_table_element_t soduko_go8[]={
    4,3,3,3
};
Path_table_element_t soduko_go9[]={
    2,3,3,3
};

void Path_table_init(Path_table_t* table,Path_table_element_t* table_data,char* name,int start_point_x,int start_point_y){
    table->table=table_data;
    table->name=name;
    table->start_point_x=start_point_x;
    table->start_point_y=start_point_y;
}
uint8_t Path_get_next_dir(Path_table_t* table)
{
    // if(table->table[table->index]==0){
    //     return 0;
    // }
    return table->table[table->index++];
}







