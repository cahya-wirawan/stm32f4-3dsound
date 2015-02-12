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
 * @file      cwHrtf.h
 * @brief     HRTF macros and structures
 * @details
 */

#ifndef __CW_HRTF_H__
#define __CW_HRTF_H__

#include "cwDsp.h"

typedef struct {
  FPComplex left[CW_DSP_FFT_SAMPLE_LENGTH], right[CW_DSP_FFT_SAMPLE_LENGTH];
} cwHrtfStereoSignal;

int cwHrtfInit(void);
void cwHrtfSoundPosition(cwHrtfStereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth);
void cwHrtfGetDb(FPComplex **hrtfLeft, FPComplex **hrtfRight, float elevation, float azimuth, int sampleLength);

#endif
