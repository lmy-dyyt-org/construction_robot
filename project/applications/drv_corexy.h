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
	
	
#ifdef __cplusplus
}
#endif
#endif
