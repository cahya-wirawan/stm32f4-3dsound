/*
 *  Hrtf.h
 *  
 *
 *  Created by Cahya Wirawan on 10/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __CW_HRTF_H__
#define __CW_HRTF_H__

#include "cwDsp.h"

typedef struct {
  FPComplex left[CW_DSP_FFT_SAMPLE_LENGTH], right[CW_DSP_FFT_SAMPLE_LENGTH];
} cwHrtfStereoSignal;

int cwHrtfInit(void);
void cwHrtfSoundPosition(cwHrtfStereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth);
void cwHrtfGetDb(FPComplex **hrtfLeft, FPComplex **hrtfRight, float elevation, float azimuth, int sampleLength);

#endif
