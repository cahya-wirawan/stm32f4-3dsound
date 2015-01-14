/*
 *  HrtfArray.h
 *  
 *
 *  Created by Cahya Wirawan on 10/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "stdint.h"

#ifndef HRTF_COMPLETE_DB

#define HRTF_ARRAY_LENGTH       128
#define HRTF_ELEVATION_MIN   0
#define HRTF_ELEVATION_MAX   0
#define HRTF_ELEVATION_STEP  10

#define HRTF_FREQUENCY        10

extern const int16_t HrtfArrayAzimuthMax[];
extern const int16_t HrtfArrayOffset[];
extern const int16_t HrtfArray[][2][HRTF_ARRAY_LENGTH];

#endif