/*
*
* File Name          :  Hrtf.c
* Description        :  HRTF Functions
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "cwHrtf.h"
#include "cwHrtfArray.h"
#include "arm_math.h"

int cwHRTFInit() {
  cwDspInit(CW_DSP_FFT_SAMPLE_LENGTH);
  return 0;
}

void cwHRTFReadDb(cwHRTFStereoSignal *hrtfDb, float elevation, float azimuth, int sampleLength) {
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

void cwHRTFSoundPosition(cwHRTFStereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth) {
  FPComplex fp1[CW_DSP_FFT_SAMPLE_LENGTH], fp2[CW_DSP_FFT_SAMPLE_LENGTH], fp3[CW_DSP_FFT_SAMPLE_LENGTH];
  
  cwHRTFStereoSignal *hrtfDb;
  
  hrtfDb = (cwHRTFStereoSignal *)stereoSignal;
  cwHRTFReadDb(hrtfDb, elevation, azimuth, length);
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
