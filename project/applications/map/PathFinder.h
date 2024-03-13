#ifndef PATHFINDER_H
#define PATHFINDER_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
typedef int Path_table_element_t;

typedef struct{
    int start_point_x;
    int start_point_y;
Path_table_element_t* table;
char* name;
    int index;

}Path_table_t;
extern Path_table_element_t soduko[];
extern Path_table_element_t transport[];
void Path_table_init(Path_table_t* table,Path_table_element_t* table_data,char* name,int start_point_x,int start_point_y);

uint8_t Path_get_next_dir(Path_table_t* table);


#ifdef __cplusplus
}
#endif
#endif