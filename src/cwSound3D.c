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
 * @file      cwSound3D.c
 * @brief     3D Sound effect calculation
 * @details
 */

#include "arm_math.h"
#include "string.h"
#include "Audio.h"
#include "cwSound3D.h"

#define MAX_SIGNAL 32767.0f
#define MIN_SIGNAL -MAX_SIGNAL
#define SOUND_FFT_SCALE 0.00001f

NUMBER_TYPE cwSound3DStereoWavBuffer[SOUND_BUFFER_LENGTH];
cwHrtfStereoSignal cwSound3DStereoSignal;
NUMBER_TYPE cwSound3DDoubleBuffer[4*CW_DSP_FFT_SAMPLE_LENGTH];

const NUMBER_TYPE cwSound3DHannWindowsFunction [] =
{
#include "cwWindowsFunction.h"
};

int16_t cwSound3DInit(void) {
  int i;
  
  for (i=0; i<4*CW_DSP_FFT_SAMPLE_LENGTH; i++) {
    cwSound3DDoubleBuffer[i] = 0;
  }
  cwHrtfInit();
  return 1;
}

void cwSound3DFillBuffer(int16_t * audioBuffer, int16_t *readPtr, float elevation, float azimuth, int16_t bufferNumber) {
  NUMBER_TYPE buffer2[CW_DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer1[2*CW_DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer2[2*CW_DSP_FFT_SAMPLE_LENGTH];

  cwSound3DWindowsFunction(buffer2, readPtr, CW_DSP_FFT_SAMPLE_LENGTH);
  
  cwSound3DToComplexBuffer(complexBuffer1, buffer2, CW_DSP_FFT_SAMPLE_LENGTH);
  
  cwHrtfSoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, CW_DSP_FFT_SAMPLE_LENGTH, elevation, azimuth);
  
  cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, CW_DSP_FFT_SAMPLE_LENGTH);
  
  if(bufferNumber==0) {
    cwSound3DCleanBuffer(cwSound3DDoubleBuffer+2*CW_DSP_FFT_SAMPLE_LENGTH, 2*CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer, complexBuffer2, 2*CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DWindowsFunction(buffer2, readPtr+CW_DSP_FFT_SAMPLE_LENGTH/2, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DToComplexBuffer(complexBuffer1, buffer2, CW_DSP_FFT_SAMPLE_LENGTH);
    cwHrtfSoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, CW_DSP_FFT_SAMPLE_LENGTH, elevation, azimuth);
    cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+CW_DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DCopyToAudioBuffer(audioBuffer, cwSound3DDoubleBuffer, 2*CW_DSP_FFT_SAMPLE_LENGTH);
  }
  else {
    cwSound3DCleanBuffer(cwSound3DDoubleBuffer, 2*CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+2*CW_DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DWindowsFunction(buffer2, readPtr+CW_DSP_FFT_SAMPLE_LENGTH/2, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DToComplexBuffer(complexBuffer1, buffer2, CW_DSP_FFT_SAMPLE_LENGTH);
    cwHrtfSoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, CW_DSP_FFT_SAMPLE_LENGTH, elevation, azimuth);
    cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+3*CW_DSP_FFT_SAMPLE_LENGTH, complexBuffer2, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer, complexBuffer2+CW_DSP_FFT_SAMPLE_LENGTH, CW_DSP_FFT_SAMPLE_LENGTH);
    cwSound3DCopyToAudioBuffer(audioBuffer, cwSound3DDoubleBuffer+2*CW_DSP_FFT_SAMPLE_LENGTH, 2*CW_DSP_FFT_SAMPLE_LENGTH);
  }
}

void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer2[i] = buffer1[i]*cwSound3DHannWindowsFunction[i];
  }
}

int32_t cwSound3DToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length) {
  int i;
  
  for (i=0; i<length; i++) {
    complexBuffer[2*i] = buffer[i];
    complexBuffer[2*i+1] = 0;
  }
  return length;
}

int32_t cwSound3DHRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, cwHrtfStereoSignal *stereoSignal, int32_t length) {
  int i;
  
  for (i=0; i<length; i++) {
    complexBuffer[2*i] = stereoSignal->left[i][0];
    complexBuffer[2*i+1] = stereoSignal->right[i][0];
  }
  return length;
}

void cwSound3DCleanBuffer(NUMBER_TYPE *buffer, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer[i] = 0;
  }
}

void cwSound3DCopyToAudioBuffer(SOUND_BUFFER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
  float32_t sig;
  
  for (i=0; i<length; i++) {
    sig = buffer1[i]*SOUND_FFT_SCALE;
    if(sig>MAX_SIGNAL)
      sig = MAX_SIGNAL;
    else
      if(sig<MIN_SIGNAL)
        sig = MIN_SIGNAL;
    buffer2[i] = (SOUND_BUFFER_TYPE) roundf(sig);
  }
}

void cwSound3DAddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer2[i] = buffer2[i] + buffer1[i];
  }
}
