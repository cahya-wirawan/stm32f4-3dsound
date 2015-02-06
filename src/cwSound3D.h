//
//  cwSound3D.h
// 
//
//

#ifndef __CW_SOUND3D__
#define __CW_SOUND3D__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "Number.h"
#include "Hrtf.h"
#include "ff.h"

#define SOUND_BUFFER_LENGTH 4*DSP_FFT_SAMPLE_LENGTH
#define SOUND_BUFFER_SAMPLE_LENGTH (DSP_FFT_SAMPLE_LENGTH+DSP_FFT_SAMPLE_LENGTH/2)
#define SOUND_BUFFER_TYPE int16_t

void cwSound3DPlayFile(char* filename);
void cwSound3DAudioCallback(void *context,int buffer);
void cwSound3DFillBuffer(int16_t * audioBuffer, int16_t *readPtr, float azimuth, int16_t bufferPosition);
int16_t cwSound3DInit(void);
void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length);
int32_t cwSound3DToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length);
int32_t cwSound3DHRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, HRTF_StereoSignal *stereoSignal, int32_t length);
void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length);
void cwSound3DCleanBuffer(NUMBER_TYPE *buffer, int32_t length);
void cwSound3DAddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void cwSound3DCopyBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void cwSound3DCopyToAudioBuffer(SOUND_BUFFER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);

#endif /* defined(__CW_SOUND3D__) */
