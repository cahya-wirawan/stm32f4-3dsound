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
 * @file      cwSound3D.h
 * @brief     The Sound macros and structures.
 * @details
 */


#ifndef __CW_SOUND3D__
#define __CW_SOUND3D__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "cwNumber.h"
#include "cwHrtf.h"
#include "ff.h"

#define SOUND_BUFFER_LENGTH 4*CW_DSP_FFT_SAMPLE_LENGTH
#define SOUND_BUFFER_SAMPLE_LENGTH (CW_DSP_FFT_SAMPLE_LENGTH+CW_DSP_FFT_SAMPLE_LENGTH/2)
#define SOUND_BUFFER_TYPE int16_t

void cwSound3DPlayFile(char* filename);
void cwSound3DAudioCallback(void *context,int buffer);
void cwSound3DFillBuffer(int16_t * audioBuffer, int16_t *readPtr, float azimuth, int16_t bufferPosition);
int16_t cwSound3DInit(void);
void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length);
int32_t cwSound3DToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length);
int32_t cwSound3DHRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, cwHrtfStereoSignal *stereoSignal, int32_t length);
void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length);
void cwSound3DCleanBuffer(NUMBER_TYPE *buffer, int32_t length);
void cwSound3DAddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void cwSound3DCopyBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void cwSound3DCopyToAudioBuffer(SOUND_BUFFER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);

#endif /* defined(__CW_SOUND3D__) */
