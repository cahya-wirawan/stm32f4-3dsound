/*
*
* File Name          :  DSP.h
* Description        :  DSP Functions
*/

#ifndef DSP_H
#define DSP_H

#include "Number.h"

#define DSP_FFT_SAMPLE_LENGTH 1024
#define DSP_FFT_WINDOW_SIZE DSP_FFT_SAMPLE_LENGTH
#define DSP_AMP_MAX 32767
#define DSP_AMP_MIN -DSP_INT16_T_MAX
#define DSP_SHIFT 4

void DSP_Init(int16_t length);
void DSP_Fft(FPComplex *output, FPComplex *input);
void DSP_Ifft(FPComplex *output, FPComplex *input);

#endif

NUMBER_TYPE DSP_SetMinMax(NUMBER_TYPE_MAX input);
void DSP_Scale(FPComplex *input, NUMBER_TYPE scale, int length);
void DSP_ArrayScale(FPComplex *output, FPComplex *input, NUMBER_TYPE scale, int length) ;
void DSP_Zeros(FPComplex *input);
void DSP_ComplexArrayMultiply(FPComplex *output, FPComplex *input1, FPComplex *input2, int length);
void DSP_ComplexArrayAdd(FPComplex *output, FPComplex *input1, FPComplex *input2, int length);
void DSP_ArrayMultiply(FPComplex *output, FPComplex *input1, NUMBER_TYPE *input2, int length);
