#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "linear_Interp.h"



int Circle_Interpolation_Creat(Interpolation_handle_t* interp)
{
	CircleInterpolation_t* Circle_Interpolation = (CircleInterpolation_t*)interp->Interpolation_driver_handle;
	Circle_Interpolation->ori = interp->original_points[0];
	Circle_Interpolation->cfg = interp->original_points[1];
}


float Circle_Interpolate(Interpolation_handle_t* interp, float x)
{
	CircleInterpolation_t* Circle_Interpolation = (CircleInterpolation_t*)interp->Interpolation_driver_handle;
	return  sqrt(1 - ((x * x) / Circle_Interpolation->cfg.x));
}

