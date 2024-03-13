/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-03-11 23:44:51
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-03-12 00:36:58
 * @FilePath: \SemanticMap\SemanticMap\SemanticMap.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef SEMANTICMAP_H
#define SEMANTICMAP_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

typedef struct Smt_map
{
    int8_t x_fr;
    int8_t y_fr;

    int8_t x_br;
    int8_t y_br;
} Smt_map_t;

extern const Smt_map_t map_soduko[4][5];
extern const Smt_map_t map_transport[7][12];

void print_map_soduko(void);
void printf_map_transport(void);

#ifdef __cplusplus
}
#endif
#endif
