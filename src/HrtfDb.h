/*
 *  HrtfDb.h
 *  
 *
 *  Created by Cahya Wirawan on 10/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HRTF_DB_H
#define HRTF_DB_H

#include "stdint.h"

#ifndef HRTF_COMPLETE_DB

#define HRTF_DB_LENGTH         1024

#define HRTF_ELEVATION_MIN   0
#define HRTF_ELEVATION_MAX   0
#define HRTF_AZIMUTH_MIN   -90
#define HRTF_AZIMUTH_MAX   90
#define HRTF_ELEVATION_STEP  10

#define HRTF_FREQUENCY        10

extern const int16_t HrtfDbAzimuthMax[];
extern const int16_t HrtfDbOffset[];
extern const int16_t HrtfDb[][2][2*HRTF_DB_LENGTH];

#endif

#endif
