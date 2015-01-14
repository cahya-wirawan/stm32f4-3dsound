/*
 *  Hrtf.h
 *  
 *
 *  Created by Cahya Wirawan on 10/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HRTF_H
#define HRTF_H

#include "DSP.h"

typedef struct {
  FPComplex left[DSP_FFT_SAMPLE_LENGTH], right[DSP_FFT_SAMPLE_LENGTH];
} HRTF_StereoSignal;

int HRTF_Init(void);
void HRTF_SoundPosition(HRTF_StereoSignal *stereoSignal, FPComplex *signal, int length, float elevation, float azimuth);
void HRTF_ReadDb(HRTF_StereoSignal *hrtfDb, float elevation, float azimuth, int sampleLength);
void HRTF_GetDb(FPComplex **hrtfLeft, FPComplex **hrtfRight, float elevation, float azimuth, int sampleLength);

#endif