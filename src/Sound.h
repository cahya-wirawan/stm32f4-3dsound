/*
*
* File Name          :  Sound.h
* Description        :  Clock Counter.
*/

#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include "Number.h"
#include "cwFS.h"
#include "Audio.h"
#include "Hrtf.h"

#define SOUND_BUFFER_LENGTH 4*DSP_FFT_SAMPLE_LENGTH
#define SOUND_BUFFER_SAMPLE_LENGTH (DSP_FFT_SAMPLE_LENGTH+DSP_FFT_SAMPLE_LENGTH/2)
#define SOUND_BUFFER_TYPE int16_t
#define HEADER_SIZE 44

/*
#define SOUND_BUFFER_TYPE_INT16_T

#ifdef SOUND_BUFFER_TYPE_INT16_T
#define SOUND_BUFFER_TYPE int16_t
#elif defined(SOUND_BUFFER_TYPE_FLOAT)
#define SOUND_BUFFER_TYPE float
#endif
*/

int16_t Sound_Init(void);
SOUND_BUFFER_TYPE *Sound_Get_AudioBuffer(void);
int32_t Sound_Get_AudioBuffer_Size(void);
int32_t Sound_Position(SOUND_BUFFER_TYPE *dest, const NUMBER_TYPE *src, int32_t srcLength, float elevation, float azimuth);
int16_t Sound_FillBuffer(FILEINFO *file_info);
int32_t Sound_Read(int16_t *dest, const int16_t *src, int32_t destLength, int32_t srcLength);
float Sound_GetAzimuth(void);
int32_t Sound_ReadWavFile_Stereo(FILEINFO *fileInfo, NUMBER_TYPE *soundBuffer, uint32_t length);
int32_t Sound_ReadWavFile_Mono_Offset(FILEINFO *fileInfo, NUMBER_TYPE *soundBuffer, uint32_t length, uint32_t offset);
int32_t Sound_Copy(uint8_t *dst, uint8_t *src, int32_t length);
int32_t Sound_Copy_16(int16_t *dst, const int16_t *src, int32_t length);
int32_t Sound_ToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length);
int32_t Sound_HRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, HRTF_StereoSignal *stereoSignal, int32_t length);
void Sound_WindowsFunction(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void Sound_CleanBuffer(NUMBER_TYPE *buffer, int32_t length);
void Sound_AddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void Sound_CopyBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length);
void Sound_FillBufferWF(FILEINFO *fileInfo, float azimuth, int16_t bufferPosition);

#endif
