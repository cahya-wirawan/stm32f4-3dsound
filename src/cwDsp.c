//
//  cwDsp.c
//  
//
//  Created by Cahya Wirawan on 08/02/15.
//
//

#include <stdint.h>
#include "cwDsp.h"
#include "Number.h"
#include "arm_math.h"


arm_status cwDspFftStatus;
arm_cfft_radix4_instance_f32 cwDspFftInstance;
arm_cfft_radix4_instance_f32 cwDspIfftInstance;
//float32_t cwDspFft_Sample[2*DSP_FFT_SAMPLE_LENGTH];
int16_t cwDspLength;

void cwDspInit(int16_t length) {
  cwDspLength = length;
  cwDspFftStatus = arm_cfft_radix4_init_f32(&cwDspFftInstance, CW_DSP_FFT_SAMPLE_LENGTH, 0, 1);
  cwDspFftStatus = arm_cfft_radix4_init_f32(&cwDspIfftInstance, CW_DSP_FFT_SAMPLE_LENGTH, 1, 1);
}

void cwDspFft(FPComplex *output, FPComplex *input) {
  arm_copy_f32((float32_t *) input, (float32_t *) output, 2*cwDspLength);
  arm_cfft_radix4_f32(&cwDspFftInstance, (float32_t *) output);
}

void cwDspIfft(FPComplex *output, FPComplex *input) {
  arm_copy_f32((float32_t *) input, (float32_t *) output, 2*cwDspLength);
  arm_cfft_radix4_f32(&cwDspIfftInstance, (float32_t *) output);
}


NUMBER_TYPE cwDspSetMinMax(NUMBER_TYPE_MAX input) {
  if(input>NUMBER_AMP_MAX)
    input=NUMBER_AMP_MAX;
  else
    if(input<NUMBER_AMP_MIN)
      input=NUMBER_AMP_MIN;
  
  return (NUMBER_TYPE) input;
}

void cwDspScale(FPComplex *input, NUMBER_TYPE scale, int length) {
#ifdef NUMBER_TYPE_INT16_T
  arm_scale_q15((q15_t *)input, length, 15, (q15_t *)input, 2*length);
#else
#ifdef NUMBER_TYPE_FLOAT
  arm_scale_f32((float32_t *)input, scale, (float32_t *)input, 2*length);
#else
  cwDspArrayScale(input, input, scale, length);
#endif
#endif
}

void cwDspArrayScale(FPComplex *output, FPComplex *input, NUMBER_TYPE scale, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {
    x = input[i][0]*scale;
    x = cwDspSetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input[i][1]*scale;
    y = cwDspSetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}

void cwDspZeros(FPComplex *input) {
  int i;
  
  for (i=0; i<cwDspLength; i++) {
    input[i][0]=0;
    input[i][1]=0;
  }
}

void cwDspComplexArrayMultiply(FPComplex *output, FPComplex *input1, FPComplex *input2, int length) {
#ifdef FIXED_POINT
  arm_cmplx_mult_cmplx_q15((q15_t *)input1, (q15_t *)input2, (q15_t *)output, length);
#else
  arm_cmplx_mult_cmplx_f32((float32_t *)input1, (float32_t *)input2, (float32_t *)output, length);
#endif
}

void cwDspComplexArrayAdd(FPComplex *output, FPComplex *input1, FPComplex *input2, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {
    x = input1[i][0]+input2[i][0];
    x = cwDspSetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input1[i][1]+input2[i][1];
    y = cwDspSetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}

void cwDspArrayMultiply(FPComplex *output, FPComplex *input1, NUMBER_TYPE *input2, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {
    x = input1[i][0]*input2[i];
    x = cwDspSetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input1[i][1]*input2[i];
    y = cwDspSetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}
