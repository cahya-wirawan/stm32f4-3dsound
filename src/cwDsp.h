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
 * @file      cwDsp.h
 * @brief     DSP macros and structures.
 * @details
 */

#ifndef __CW_DSP__
#define __CW_DSP__

#include "cwNumber.h"

#define CW_DSP_FFT_SAMPLE_LENGTH 1024
#define CW_DSP_FFT_WINDOW_SIZE CW_DSP_FFT_SAMPLE_LENGTH
#define CW_DSP_AMP_MAX 32767
#define CW_DSP_AMP_MIN -CW_DSP_INT16_T_MAX
#define CW_DSP_SHIFT 4

void cwDspInit(int16_t length);
void cwDspFft(FPComplex *output, FPComplex *input);
void cwDspIfft(FPComplex *output, FPComplex *input);
NUMBER_TYPE cwDspSetMinMax(NUMBER_TYPE_MAX input);
void cwDspScale(FPComplex *input, NUMBER_TYPE scale, int length);
void cwDspArrayScale(FPComplex *output, FPComplex *input, NUMBER_TYPE scale, int length) ;
void cwDspZeros(FPComplex *input);
void cwDspComplexArrayMultiply(FPComplex *output, FPComplex *input1, FPComplex *input2, int length);
void cwDspComplexArrayAdd(FPComplex *output, FPComplex *input1, FPComplex *input2, int length);
void cwDspArrayMultiply(FPComplex *output, FPComplex *input1, NUMBER_TYPE *input2, int length);

#endif /* defined(__CW_DSP__) */
