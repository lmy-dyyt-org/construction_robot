/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-05-01 16:48:10
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-01 17:28:59
 * @FilePath: \project\applications\interpolation\itpl_circle.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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

