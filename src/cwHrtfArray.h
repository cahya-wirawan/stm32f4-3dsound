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
 * @file      cwHrtfArray.h
 * @brief     HRTF Array macros and structures
 * @details   
 */

#include "stdint.h"

// Choose only one of the following HRTF Database:

#define CW_HRTF_KEMAR_DIFFUSE
//#define CW_HRTF_KEMAR_COMPLETE
//#define CW_HRTF_PKU_IOA_20

#define CW_HRTF_ELEVATION_MIN   -40
#define CW_HRTF_ELEVATION_MAX    90
#define CW_HRTF_ELEVATION_STEP   10
#define CW_HRTF_AZIMUTH_MIN     -90
#define CW_HRTF_AZIMUTH_MAX      90

#define CW_HRTF_FREQUENCY        10

#ifdef CW_HRTF_KEMAR_COMPLETE

#include "cwHrtfKemarCompact.h"

#else

#ifdef CW_HRTF_KEMAR_DIFFUSE

#include "cwHrtfKemarDiffuse.h"

#else

#ifdef CW_HRTF_PKU_IOA_20

#include "cwHrtfPKU-IOA-20.h"

#endif /*CW_HRTF_PKU_IOA_20#*/

#endif /*CW_HRTF_KEMAR_DIFFUSE#*/

#endif /*CW_HRTF_KEMAR_COMPLETE*/


extern const int16_t cwHrtfArrayAzimuthMax[];
extern const int16_t cwHrtfArrayOffset[];
extern const int16_t cwHrtfArray[][2][CW_HRTF_ARRAY_LENGTH];
