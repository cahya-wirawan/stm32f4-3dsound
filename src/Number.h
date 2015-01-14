//
//  Number.h
//  SoundWave
//
//  Created by Cahya Wirawan on 1/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>

#define NUMBER_TYPE_FLOAT
#define HRTF_DB_ENABLED
#define HRTF_DB_SCALING_2

#ifdef NUMBER_TYPE_INT16_T
  #define DSP_CMSIS
  #define FIXED_POINT 16
  #define NUMBER_TYPE int16_t
  #define NUMBER_TYPE_MAX int32_t
#else
  #ifdef NUMBER_TYPE_INT32_T
    #define DSP_KISS
    #define FIXED_POINT 32
    #define NUMBER_TYPE int32_t
  #else
    #undef HRTF_DB_ENABLED
    #define NUMBER_TYPE_MAX double
    #ifdef NUMBER_TYPE_FLOAT
      #define DSP_CMSIS
      #define NUMBER_TYPE float
    #else
      #ifdef NUMBER_TYPE_DOUBLE
        #define DSP_FFTW3
        #define NUMBER_TYPE double
      #endif
    #endif
  #endif
#endif

#ifdef FIXED_POINT
  #define NUMBER_AMP_MAX 32767
#else
  #define NUMBER_AMP_MAX 2147483647
#endif
#define NUMBER_AMP_MIN -NUMBER_AMP_MAX

//#define NUMBER_WAV_SCALE (1.0f/(float)32768.0f)

typedef NUMBER_TYPE FPComplex[2];

#endif
