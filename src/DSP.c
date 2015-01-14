
/*
*
* File Name          :  DSP.c
* Description        :  DSP Functions
*/


#include <stdint.h>
#include "DSP.h"
#include "Number.h"
#include "arm_math.h"
#include "ClockCounter.h"
#ifdef CIRCLEOS
#include "circle_api.h"
#endif

#ifdef DSP_CMSIS

#ifdef FIXED_POINT

arm_status _DSP_fft_status;
arm_cfft_radix4_instance_q15 _DSP_fft_instance;
arm_cfft_radix4_instance_q15 _DSP_ifft_instance;
//q15_t _DSP_FFT_Sample[2*DSP_FFT_SAMPLE_LENGTH];

int16_t _DSP_length;
void DSP_Init(int16_t length) {
  _DSP_length = length;
  _DSP_fft_status = arm_cfft_radix4_init_q15(&_DSP_fft_instance, DSP_FFT_SAMPLE_LENGTH, 0, 1);
  _DSP_fft_status = arm_cfft_radix4_init_q15(&_DSP_ifft_instance, DSP_FFT_SAMPLE_LENGTH, 1, 1);
}

void DSP_Fft(FPComplex *output, FPComplex *input) {
    arm_copy_q15((q15_t *) input, (q15_t *) output, 2*_DSP_length);
    arm_cfft_radix4_q15(&_DSP_fft_instance, (q15_t *) output);
    DSP_Scale(output, _DSP_length, _DSP_length);
}

void DSP_Ifft(FPComplex *output, FPComplex *input) {
    arm_copy_q15((q15_t *) input, (q15_t *) output, 2*_DSP_length);
    arm_cfft_radix4_q15(&_DSP_ifft_instance, (q15_t *) output);
}

#else // if not FIXED_POINT

arm_status _DSP_fft_status;
arm_cfft_radix4_instance_f32 _DSP_fft_instance;
arm_cfft_radix4_instance_f32 _DSP_ifft_instance;
//float32_t _DSP_FFT_Sample[2*DSP_FFT_SAMPLE_LENGTH];
int16_t _DSP_length;

void DSP_Init(int16_t length) {
  _DSP_length = length;
  _DSP_fft_status = arm_cfft_radix4_init_f32(&_DSP_fft_instance, DSP_FFT_SAMPLE_LENGTH, 0, 1);
  _DSP_fft_status = arm_cfft_radix4_init_f32(&_DSP_ifft_instance, DSP_FFT_SAMPLE_LENGTH, 1, 1);
}

void DSP_Fft(FPComplex *output, FPComplex *input) {
    arm_copy_f32((float32_t *) input, (float32_t *) output, 2*_DSP_length);
    arm_cfft_radix4_f32(&_DSP_fft_instance, (float32_t *) output);
    //DSP_Scale(output, _DSP_length, _DSP_length);
}

void DSP_Ifft(FPComplex *output, FPComplex *input) {
    arm_copy_f32((float32_t *) input, (float32_t *) output, 2*_DSP_length);
    arm_cfft_radix4_f32(&_DSP_ifft_instance, (float32_t *) output);
}

#endif // FIXED_POINT

#else

#ifdef DSP_ES

int16_t _DSP_length;
void DSP_Init(int16_t length) {
  _DSP_length = length;
}

void DSP_Fft(FPComplex *output, FPComplex *input) { 
  fftR4(output, input, _DSP_length);
#ifdef FIXED_POINT
  DSP_Scale(output, _DSP_length, _DSP_length);
#else
  DSP_Scale(output, 1./_DSP_length, _DSP_length);
#endif
}

void DSP_Ifft(FPComplex *output, FPComplex *input) {
    ifftR4(output, input, _DSP_length);
#ifdef FIXED_POINT
#else
  DSP_Scale(output, 1./_DSP_length, _DSP_length);
#endif
}

#endif
#endif

NUMBER_TYPE DSP_SetMinMax(NUMBER_TYPE_MAX input) {
  if(input>NUMBER_AMP_MAX)
    input=NUMBER_AMP_MAX;
  else
    if(input<NUMBER_AMP_MIN)
      input=NUMBER_AMP_MIN;
  
  return (NUMBER_TYPE) input;
}

void DSP_Scale(FPComplex *input, NUMBER_TYPE scale, int length) {
#ifdef NUMBER_TYPE_INT16_T
  arm_scale_q15((q15_t *)input, length, 15, (q15_t *)input, 2*length);
#else
#ifdef NUMBER_TYPE_FLOAT
  arm_scale_f32((float32_t *)input, scale, (float32_t *)input, 2*length);
#else
  DSP_ArrayScale(input, input, scale, length);
#endif
#endif
}

void DSP_ArrayScale(FPComplex *output, FPComplex *input, NUMBER_TYPE scale, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {    
    x = input[i][0]*scale;
    x = DSP_SetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input[i][1]*scale;
    y = DSP_SetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}

void DSP_Zeros(FPComplex *input) {
  int i;
  
  for (i=0; i<_DSP_length; i++) {
    input[i][0]=0;
    input[i][1]=0;
  }
}

void DSP_ComplexArrayMultiply(FPComplex *output, FPComplex *input1, FPComplex *input2, int length) {    
#ifdef FIXED_POINT
  arm_cmplx_mult_cmplx_q15((q15_t *)input1, (q15_t *)input2, (q15_t *)output, length);
#else
  arm_cmplx_mult_cmplx_f32((float32_t *)input1, (float32_t *)input2, (float32_t *)output, length);  
#endif
}

void DSP_ComplexArrayAdd(FPComplex *output, FPComplex *input1, FPComplex *input2, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {
    x = input1[i][0]+input2[i][0];
    x = DSP_SetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input1[i][1]+input2[i][1];
    y = DSP_SetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}

void DSP_ArrayMultiply(FPComplex *output, FPComplex *input1, NUMBER_TYPE *input2, int length) {
  int i;
  NUMBER_TYPE_MAX x, y;
  
  for( i = 0 ; i < length ; i++ ) {    
    x = input1[i][0]*input2[i];
    x = DSP_SetMinMax(x);
    output[i][0] = (NUMBER_TYPE) x;
    
    y = input1[i][1]*input2[i];
    y = DSP_SetMinMax(y);
    output[i][1] = (NUMBER_TYPE) y;
  }
}
