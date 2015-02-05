/*
*
* File Name          :  Hrtf.c
* Description        :  HRTF Functions
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "Hrtf.h"
#include "HrtfArray.h"
#include "HrtfDb.h"
#include "arm_math.h"

int HRTF_Init() {
  DSP_Init(DSP_FFT_SAMPLE_LENGTH);
  return 0;
}

void HRTF_ReadDb(HRTF_StereoSignal *hrtfDb, float elevation, float azimuth, int sampleLength) {
  int i, flip, hrtfElevationIndex, hrtfAzimuthIndex;
  const int16_t *tmpArray, *leftArray, *rightArray;
  
  if(azimuth<0)
    flip = 1;
  else
    flip = 0;
  if(elevation<HRTF_ELEVATION_MIN)
    elevation = HRTF_ELEVATION_MIN;
  else if(elevation>HRTF_ELEVATION_MAX)
    elevation = HRTF_ELEVATION_MAX;
  
  //printf("HRTF_ReadDb 1\r\n");
  hrtfElevationIndex = (int) round((elevation-HRTF_ELEVATION_MIN)/10.0);
  azimuth = abs(azimuth);  
  if(azimuth>HrtfArrayAzimuthMax[hrtfElevationIndex])
    azimuth = HrtfArrayAzimuthMax[hrtfElevationIndex];
  //printf("HRTF_ReadDb 2\r\n");
  
  hrtfAzimuthIndex = (int) round(azimuth*(HrtfArrayOffset[hrtfElevationIndex+1]-HrtfArrayOffset[hrtfElevationIndex]-1)/HrtfArrayAzimuthMax[hrtfElevationIndex]);
  //printf("HRTF_ReadDb 3\r\n");
  
  leftArray = &HrtfArray[HrtfArrayOffset[hrtfElevationIndex]+hrtfAzimuthIndex][0][0];
  rightArray = &HrtfArray[HrtfArrayOffset[hrtfElevationIndex]+hrtfAzimuthIndex][1][0];
  //printf("HRTF_ReadDb 4\r\n");
  if(flip) {
    tmpArray = leftArray;
    leftArray = rightArray;
    rightArray = tmpArray;
  }
  for( i = 0 ; i < HRTF_ARRAY_LENGTH ; i++ ) {
    hrtfDb->left[i][0] = leftArray[i];
    hrtfDb->left[i][1] = 0;
    hrtfDb->right[i][0] = rightArray[i];
    hrtfDb->right[i][1] = 0;
    //printf("HRTF_ReadDb 5: %d\r\n", i);
    //printf("hrtf %d: %d, %d\n", i, leftArray[i], rightArray[i]);
  }
  
  //printf("HRTF_ReadDb 5.1: %d\r\n", i);
  
  for( i = HRTF_ARRAY_LENGTH ; i < sampleLength ; i++ ) {
    hrtfDb->left[i][0] = 0.0;
    hrtfDb->left[i][1] = 0.0;
    hrtfDb->right[i][0] = 0.0;
    hrtfDb->right[i][1] = 0.0;
    //printf("HRTF_ReadDb 6: %d/%d\r\n", i, sampleLength);
  }

  //printf("HRTF_ReadDb 7: %d\r\n", i);
}

void HRTF_GetDb(FPComplex **hrtfLeft, FPComplex **hrtfRight, float elevation, float azimuth, int sampleLength) {
  int flip, hrtfElevationIndex, hrtfAzimuthIndex;
  const int16_t *tmpArray, *leftArray, *rightArray;
  
  if(azimuth<0)
    flip = 1;
  else
    flip = 0;
  if(elevation<HRTF_ELEVATION_MIN)
    elevation = HRTF_ELEVATION_MIN;
  else if(elevation>HRTF_ELEVATION_MAX)
    elevation = HRTF_ELEVATION_MAX;
  
  hrtfElevationIndex = (int) round((elevation-HRTF_ELEVATION_MIN)/10.0);
  azimuth = abs(azimuth);  
  if(azimuth>HrtfDbAzimuthMax[hrtfElevationIndex])
    azimuth = HrtfDbAzimuthMax[hrtfElevationIndex];
  
  hrtfAzimuthIndex = (int) round(azimuth*(HrtfDbOffset[hrtfElevationIndex+1]-HrtfDbOffset[hrtfElevationIndex]-1)/HrtfDbAzimuthMax[hrtfElevationIndex]);
  
  leftArray = &HrtfDb[HrtfDbOffset[hrtfElevationIndex]+hrtfAzimuthIndex][0][0];
  rightArray = &HrtfDb[HrtfDbOffset[hrtfElevationIndex]+hrtfAzimuthIndex][1][0];
  if(flip) {
    tmpArray = leftArray;
    leftArray = rightArray;
    rightArray = tmpArray;
  }
  *hrtfLeft = (FPComplex *) leftArray;
  *hrtfRight = (FPComplex *) rightArray;
}

#ifdef HRTF_DB_ENABLED

void HRTF_SoundPosition(HRTF_StereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth) {
  printf("HRTF_SoundPosition  HRTF_DB_ENABLED 1\r\n");
  FPComplex hrtfSignal[DSP_FFT_SAMPLE_LENGTH], *hrtfLeft, *hrtfRight, hrtfOutputSignal[DSP_FFT_SAMPLE_LENGTH];
  
  HRTF_GetDb(&hrtfLeft, &hrtfRight, elevation, azimuth, length);
  DSP_Zeros(hrtfSignal);
  DSP_Fft(hrtfSignal, signal);

  DSP_Zeros(hrtfOutputSignal);
  DSP_ComplexArrayMultiply(hrtfOutputSignal, hrtfLeft, hrtfSignal, length);
  DSP_Ifft(stereoSignal->left, hrtfOutputSignal);

  DSP_Zeros(hrtfOutputSignal);
  DSP_ComplexArrayMultiply(hrtfOutputSignal, hrtfRight, hrtfSignal, length);
  DSP_Ifft(stereoSignal->right, hrtfOutputSignal);
}

#else

void HRTF_SoundPosition(HRTF_StereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth) {
  //printf("HRTF_SoundPosition disabled\r\n");
  FPComplex fp1[DSP_FFT_SAMPLE_LENGTH], fp2[DSP_FFT_SAMPLE_LENGTH], fp3[DSP_FFT_SAMPLE_LENGTH];
  //int i;
  
  //printf("HRTF_SoundPosition 1\r\n");
  HRTF_StereoSignal *hrtfDb;

  hrtfDb = (HRTF_StereoSignal *)stereoSignal;
  //printf("HRTF_SoundPosition 2\r\n");
  HRTF_ReadDb(hrtfDb, elevation, azimuth, length);
  //printf("HRTF_SoundPosition 3\r\n");
  DSP_Zeros(fp1);
  //printf("HRTF_SoundPosition 4\r\n");
  DSP_Fft(fp1, signal);
  //printf("HRTF_SoundPosition 5\r\n");

  /*
  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("fft signal %d: %d %d\n", i, fp1[i][0], fp1[i][1]);
#else
    printf("fft signal %d: %f %f\n", i, fp1[i][0], fp1[i][1]);
#endif
  }
 */
#ifdef TEST_REAL
  /** Test **/
  DSP_Zeros(fp2);
  printf("HRTF_SoundPosition 6\r\n");
  DSP_Ifft(fp2, fp1);
  printf("HRTF_SoundPosition 7\r\n");

  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("Ifft signal %d: %d %d\n", i, fp2[i][0], fp2[i][1]);
#else
    printf("Ifft signal %d: %f %f\n", i, fp2[i][0], fp2[i][1]);
#endif
  }
  /** Test **/
#endif
  
  //printf("HRTF_SoundPosition 8\r\n");
  DSP_Zeros(fp2);
  //printf("HRTF_SoundPosition 9\r\n");
  DSP_Fft(fp2, hrtfDb->left);
  //printf("HRTF_SoundPosition 10\r\n");
  /*
  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("fft left %d: %d %d\n", i, fp2[i][0], fp2[i][1]);
#else
    printf("fft left %d: %f %f\n", i, fp2[i][0], fp2[i][1]);
#endif
  }
  */
  DSP_Zeros(fp3);
  //printf("HRTF_SoundPosition 11\r\n");
  DSP_ComplexArrayMultiply(fp3, fp2, fp1, length);
  //printf("HRTF_SoundPosition 12\r\n");
  DSP_Ifft(stereoSignal->left, fp3);
  //printf("HRTF_SoundPosition 13\r\n");
  /*
  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("stereoSignal->left %d: %d %d\n", i, stereoSignal->left[i][0], stereoSignal->left[i][1]);
#else
    printf("stereoSignal->left  %d: %f %f\n", i, stereoSignal->left[i][0], stereoSignal->left[i][1]);
#endif
  }
  */
  DSP_Fft(fp2, hrtfDb->right);
  //printf("HRTF_SoundPosition 14\r\n");
  /*
  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("fft right %d: %d %d\n", i, fp2[i][0], fp2[i][1]);
#else
    printf("fft right %d: %f %f\n", i, fp2[i][0], fp2[i][1]);
#endif
  }
  */
  DSP_ComplexArrayMultiply(fp3, fp2, fp1, length);
  //printf("HRTF_SoundPosition 15\r\n");
  DSP_Ifft(stereoSignal->right, fp3);
  //printf("HRTF_SoundPosition 16\r\n");
  /*
  for (i=0; i<DSP_FFT_SAMPLE_LENGTH; i++) {
#ifdef FIXED_POINT
    printf("stereoSignal->right %d: %d %d\n", i, stereoSignal->right[i][0], stereoSignal->right[i][1]);
#else
    printf("stereoSignal->right  %d: %f %f\n", i, stereoSignal->right[i][0], stereoSignal->right[i][1]);
#endif
  }
  */
}

#endif
