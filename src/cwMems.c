/*
 STM32F4-3DSound - Copyright (C) 2015 Cahya Wirawan.
 
 This file is part of STM32F4-3DSound
 
 STM32F4-3DSound is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 STM32F4-3DSound is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file      cwMems.c
 * @brief     Interface to the LIS302DL/LIS3DSH three axes linear accelerometer (MEMS motion sensor).
 * @details
 */

#include "stdio.h"
#include "cwMems.h"
#include "tm_stm32f4_lis302dl_lis3dsh.h"

#define CW_MEMS_AZIMUTH_MAX    90
#define CW_MEMS_ELEVATION_MAX  90

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
  azimuth *= CW_MEMS_AZIMUTH_MAX;
  return azimuth;
}

void cwMemsGetPosition(float *elevation, float *azimuth) {
  
  TM_LIS302DL_LIS3DSH_t axesData;
  TM_LIS302DL_LIS3DSH_ReadAxes(&axesData);
  
  *azimuth = (float)(axesData.X - cwMemsOffset.X)/CW_MEMS_MAX_VALUE;
  *azimuth = (*azimuth>1.0)? 1.0:((*azimuth<-1.0)?-1.0:*azimuth);
  *azimuth *= CW_MEMS_AZIMUTH_MAX;
  
  *elevation = (float)(axesData.Y - cwMemsOffset.Y)/CW_MEMS_MAX_VALUE;
  *elevation = (*elevation>1.0)? 1.0:((*elevation<-1.0)?-1.0:*elevation);
  *elevation *= CW_MEMS_ELEVATION_MAX;
}