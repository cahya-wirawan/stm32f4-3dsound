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
 * @file      cwNumber.h
 * @brief     The main macro and definition for number
 * @details
 */

#ifndef __CW_NUMBER_H__
#define __CW_NUMBER_H__

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
