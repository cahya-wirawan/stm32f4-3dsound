/*
 *  cwMems.c
 *
 *  Author: Cahya Wirawan
 */

#include "stdio.h"
#include "cwMems.h"
#include "HrtfDb.h"
#include "tm_stm32f4_lis302dl_lis3dsh.h"

TM_LIS302DL_LIS3DSH_t cwMemsOffset;

void cwMemsInit(void) {
  TM_LIS302DL_LIS3DSH_ReadAxes(&cwMemsOffset);
}

float cwMemsGetAzimuth(void) {
  float azimuth;
  
  TM_LIS302DL_LIS3DSH_t axesData;
  TM_LIS302DL_LIS3DSH_ReadAxes(&axesData);
  
  azimuth = (float)(axesData.X - cwMemsOffset.X)/CW_MEMS_MAX_VALUE;
  azimuth = (azimuth>1.0)? 1.0:((azimuth<-1.0)?-1.0:azimuth);
  azimuth *= HRTF_AZIMUTH_MAX;
  return azimuth;
}
