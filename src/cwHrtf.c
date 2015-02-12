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
 * @file      cwHrtf.c
 * @brief     Human Related Transfer Functions (HRTF) calculation
 * @details
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "cwHrtf.h"
#include "cwHrtfArray.h"
#include "arm_math.h"

int cwHrtfInit() {
  cwDspInit(CW_DSP_FFT_SAMPLE_LENGTH);
  return 0;
}

void cwHrtfReadDb(cwHrtfStereoSignal *hrtfDb, float elevation, float azimuth, int sampleLength) {
  int i, flip, hrtfElevationIndex, hrtfAzimuthIndex;
  const int16_t *tmpArray, *leftArray, *rightArray;
  
  if(azimuth<0)
    flip = 1;
  else
    flip = 0;
  if(elevation<CW_HRTF_ELEVATION_MIN)
    elevation = CW_HRTF_ELEVATION_MIN;
  else if(elevation>CW_HRTF_ELEVATION_MAX)
    elevation = CW_HRTF_ELEVATION_MAX;
  
  hrtfElevationIndex = (int) round((elevation-CW_HRTF_ELEVATION_MIN)/10.0);
  azimuth = abs(azimuth);
  if(azimuth>cwHrtfArrayAzimuthMax[hrtfElevationIndex])
    azimuth = cwHrtfArrayAzimuthMax[hrtfElevationIndex];
  
  hrtfAzimuthIndex = (int) round(azimuth*(cwHrtfArrayOffset[hrtfElevationIndex+1]-cwHrtfArrayOffset[hrtfElevationIndex]-1)/cwHrtfArrayAzimuthMax[hrtfElevationIndex]);
  
  leftArray = &cwHrtfArray[cwHrtfArrayOffset[hrtfElevationIndex]+hrtfAzimuthIndex][0][0];
  rightArray = &cwHrtfArray[cwHrtfArrayOffset[hrtfElevationIndex]+hrtfAzimuthIndex][1][0];
  if(flip) {
    tmpArray = leftArray;
    leftArray = rightArray;
    rightArray = tmpArray;
  }
  for( i = 0 ; i < CW_HRTF_ARRAY_LENGTH ; i++ ) {
    hrtfDb->left[i][0] = leftArray[i];
    hrtfDb->left[i][1] = 0;
    hrtfDb->right[i][0] = rightArray[i];
    hrtfDb->right[i][1] = 0;
  }
  
  for( i = CW_HRTF_ARRAY_LENGTH ; i < sampleLength ; i++ ) {
    hrtfDb->left[i][0] = 0.0;
    hrtfDb->left[i][1] = 0.0;
    hrtfDb->right[i][0] = 0.0;
    hrtfDb->right[i][1] = 0.0;
  }
}

void cwHrtfSoundPosition(cwHrtfStereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth) {
  FPComplex fp1[CW_DSP_FFT_SAMPLE_LENGTH], fp2[CW_DSP_FFT_SAMPLE_LENGTH], fp3[CW_DSP_FFT_SAMPLE_LENGTH];
  
  cwHrtfStereoSignal *hrtfDb;
  
  hrtfDb = (cwHrtfStereoSignal *)stereoSignal;
  cwHrtfReadDb(hrtfDb, elevation, azimuth, length);
  cwDspZeros(fp1);
  cwDspFft(fp1, signal);
  
  cwDspZeros(fp2);
  cwDspFft(fp2, hrtfDb->left);
  
  cwDspZeros(fp3);
  cwDspComplexArrayMultiply(fp3, fp2, fp1, length);
  cwDspIfft(stereoSignal->left, fp3);
  cwDspFft(fp2, hrtfDb->right);
  cwDspComplexArrayMultiply(fp3, fp2, fp1, length);
  cwDspIfft(stereoSignal->right, fp3);
}
