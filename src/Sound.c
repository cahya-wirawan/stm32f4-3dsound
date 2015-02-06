/*
 *
 * File Name          :  Sound.c
 * Description        :  Sound Functions
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"
#include "stm32f4xx.h"
#include "Sound.h"
//#include "WavFile.h"
#include "HrtfDb.h"
#include "HrtfArray.h"
#include "cwFS.h"
#include "cwMems.h"

//#define SINWAVE_STEREO
//#define SOUND_WAV_ENABLE
#define SOUND_WINDOWS_FUNCTION_ENABLED

#ifdef FIXED_POINT
#  define MAX_SIGNAL 32767
#  define MIN_SIGNAL -MAX_SIGNAL
#  define SOUND_FFT_SCALE 128
#else
#  define MAX_SIGNAL 32767.0f
#  define MIN_SIGNAL -MAX_SIGNAL
#  define SOUND_FFT_SCALE 0.00001f
#endif

//#define MAX_AXES 55

//float gAzimuth;
//int8_t _Sound_fEOF; //FlagEndOfFile
///AUDIO_PlaybackBuffer_Status _Sound_BufferStatus;
///SOUND_BUFFER_TYPE _Sound_AudioBuffer[SOUND_BUFFER_LENGTH];
///NUMBER_TYPE _Sound_StereoWavBuffer[SOUND_BUFFER_LENGTH];
///HRTF_StereoSignal _Sound_StereoSignal;
///NUMBER_TYPE _Sound_DoubleBuffer[4*DSP_FFT_SAMPLE_LENGTH];
//uint32_t _Sound_FileOffset;

//extern volatile int cwSFBytesLeft;
//extern char *cwSFReadPtr;

const NUMBER_TYPE _Sound_Sinwave [] =
{
#include "Sinwave.h"
};


int32_t Sound_Read(int16_t *dest, const int16_t *src, int32_t destLength, int32_t srcLength) {
  int32_t i, j, blockLength, currentPos;
  blockLength = destLength/srcLength;
  currentPos = 0;
  for(i=0; i<blockLength; i++) {
    for(j=0; j<srcLength; j++) {
      dest[currentPos] = src[j];
      currentPos++;
    }
  }
  return currentPos;
}

int32_t Sound_Copy(uint8_t *dst, uint8_t *src, int32_t length) {
  int32_t i;

  for(i=0; i<length; i++) {
    dst[i] = src[i];
  }

  return i;
}

int32_t Sound_Copy_16(int16_t *dst, const int16_t *src, int32_t length) {
  int32_t i;

  for(i=0; i<length; i++) {
    dst[i] = src[i];
  }

  return i;
}

float Sound_GetAzimuth() {
  float azimuth = cwMemsGetAzimuth();
  printf("azimuth: %f\r\n", (double)azimuth);
  return azimuth;
}


