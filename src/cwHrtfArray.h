/*
 *  cwHrtfArray.h
 */

#include "stdint.h"

#ifndef CW_HRTF_COMPLETE_DB

#define CW_HRTF_ARRAY_LENGTH       128
#define CW_HRTF_ELEVATION_MIN   0
#define CW_HRTF_ELEVATION_MAX   0
#define CW_HRTF_ELEVATION_STEP  10
#define CW_HRTF_AZIMUTH_MIN   -90
#define CW_HRTF_AZIMUTH_MAX   90

#define CW_HRTF_FREQUENCY        10

extern const int16_t cwHrtfArrayAzimuthMax[];
extern const int16_t cwHrtfArrayOffset[];
extern const int16_t cwHrtfArray[][2][CW_HRTF_ARRAY_LENGTH];

#endif