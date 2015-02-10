//
//  cwDsp.h
//  
//
//  Created by Cahya Wirawan on 08/02/15.
//
//

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
