#ifndef DRV_COREXY_H
#define DRV_COREXY_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
	
	
typedef struct
{
    float x; //单位m
    float y;
}corexy_t;

extern corexy_t corexy;
extern corexy_t real_corexy;
	

int corexy_init(corexy_t *corexy);
int corexy_relative_move(corexy_t *corexy,float x,float y);
int corexy_absolute_move(corexy_t *corexy,float x,float y);

#ifdef __cplusplus
}
#endif
#endif
