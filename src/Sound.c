/*
 *
 * File Name          :  Sound.c
 * Description        :  Sound Functions
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"
#include "stm32f4xx.h"
#include "Sound.h"
//#include "WavFile.h"
#include "HrtfDb.h"
#include "HrtfArray.h"
#include "cwFS.h"
#include "cwMems.h"

//#define SINWAVE_STEREO
//#define SOUND_WAV_ENABLE
#define SOUND_WINDOWS_FUNCTION_ENABLED

#ifdef FIXED_POINT
#  define MAX_SIGNAL 32767
#  define MIN_SIGNAL -MAX_SIGNAL
#  define SOUND_FFT_SCALE 128
#else
#  define MAX_SIGNAL 32767.0f
#  define MIN_SIGNAL -MAX_SIGNAL
#  define SOUND_FFT_SCALE 0.00001f
#endif

#define MAX_AXES 55

float gAzimuth;
int8_t _Sound_fEOF; //FlagEndOfFile
///AUDIO_PlaybackBuffer_Status _Sound_BufferStatus;
///SOUND_BUFFER_TYPE _Sound_AudioBuffer[SOUND_BUFFER_LENGTH];
///NUMBER_TYPE _Sound_StereoWavBuffer[SOUND_BUFFER_LENGTH];
///HRTF_StereoSignal _Sound_StereoSignal;
///NUMBER_TYPE _Sound_DoubleBuffer[4*DSP_FFT_SAMPLE_LENGTH];
uint32_t _Sound_FileOffset;

extern volatile int cwSFBytesLeft;
extern char *cwSFReadPtr;

#if 0
int16_t Sound_Init(void) {
  int i;

  for (i=0; i<4*DSP_FFT_SAMPLE_LENGTH; i++) {
    _Sound_DoubleBuffer[i] = 0;
  }
  ///_Sound_BufferStatus = LOW_EMPTY | HIGH_EMPTY ;
  _Sound_fEOF = 0;
  _Sound_FileOffset = 0;
  HRTF_Init();
  return 1;
}
#endif

const NUMBER_TYPE _Sound_HannWindowsFunction [] =
{
#include "WindowsFunction.h"
};

const NUMBER_TYPE _Sound_Sinwave [] =
{
#include "Sinwave.h"
};

#if 0
int16_t *Sound_Get_AudioBuffer(void) {
  return _Sound_AudioBuffer;
}

int32_t Sound_Get_AudioBuffer_Size(void) { 
  return SOUND_BUFFER_LENGTH;
}
#endif

int32_t Sound_Read(int16_t *dest, const int16_t *src, int32_t destLength, int32_t srcLength) {
  int32_t i, j, blockLength, currentPos;
  blockLength = destLength/srcLength;
  currentPos = 0;
  for(i=0; i<blockLength; i++) {
    for(j=0; j<srcLength; j++) {
      dest[currentPos] = src[j];
      currentPos++;
    }
  }
  return currentPos;
}

int32_t Sound_Copy(uint8_t *dst, uint8_t *src, int32_t length) {
  int32_t i;

  for(i=0; i<length; i++) {
    dst[i] = src[i];
  }

  return i;
}

int32_t Sound_Copy_16(int16_t *dst, const int16_t *src, int32_t length) {
  int32_t i;

  for(i=0; i<length; i++) {
    dst[i] = src[i];
  }

  return i;
}

float Sound_GetAzimuth() {
  float azimuth = cwMemsGetAzimuth();
  printf("azimuth: %f\r\n", (double)azimuth);
  return azimuth;
}

#if 0

int32_t Sound_Position(SOUND_BUFFER_TYPE *dest, const NUMBER_TYPE *src,
    int32_t srcLength, float elevation, float azimuth) {
  int32_t i, currentPos;
  static float32_t minSignal, maxSignal;
#ifdef FIXED_POINT
  int32_t sig;
#else
  float32_t sig;
#endif

///  STOPWATCH_START

  HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) src, srcLength, 0, azimuth);

  ///  UTIL_int2str( &str1[0], (int)round(azimuth), 5, 1 );
  ///  DRAW_DisplayString( 0, 90, str1, 6);

  minSignal = 0; maxSignal = 0;
  currentPos = 0;
  for(i=0; i<srcLength; i++) {

    sig = _Sound_StereoSignal.left[i][0]*SOUND_FFT_SCALE;
    if(sig>MAX_SIGNAL)
      sig = MAX_SIGNAL;
    else
      if(sig<MIN_SIGNAL)
        sig = MIN_SIGNAL;

    if(sig < minSignal)
      minSignal = sig;
    if(sig > maxSignal)
      maxSignal = sig;

#ifdef FIXED_POINT
    dest[currentPos] = (SOUND_BUFFER_TYPE) sig;
#else
    dest[currentPos] = (SOUND_BUFFER_TYPE) roundf(sig);
#endif
    currentPos++;

    sig = _Sound_StereoSignal.right[i][0]*SOUND_FFT_SCALE;
    if(sig>MAX_SIGNAL)
      sig = MAX_SIGNAL;
    else
      if(sig<MIN_SIGNAL)
        sig = MIN_SIGNAL;
#ifdef FIXED_POINT
    dest[currentPos] = (SOUND_BUFFER_TYPE) sig;
#else
    dest[currentPos] = (SOUND_BUFFER_TYPE) roundf(sig);
#endif
    currentPos++;
  }

  //Sound_Copy_16(dest, _Sound_Sinwave, 2048);
///  STOPWATCH_STOP
  ///CycleCounter_Print(0, 130, 10);
  printf("minSignal:%d, maxSignal:%d\n", (int)minSignal, (int)maxSignal);
  return currentPos;
}
#endif

#if 0
int16_t Sound_FillBuffer(FILEINFO *fileInfo) {
#ifndef SOUND_WINDOWS_FUNCTION_ENABLED
  int32_t half_audio_buffer = SOUND_BUFFER_LENGTH/2;
#endif
  _Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(0);
  float azimuth;
  static float oldAzimuth = 180.0;
  static int16_t soundCounter = 0;

  if ( _Sound_fEOF ) {
    /****
        if ( ( _Sound_BufferStatus & LOW_EMPTY ) && ( _Sound_BufferStatus & HIGH_EMPTY ) ) {
            DRAW_Clear();
            AUDIO_Playback_Stop();
            return MENU_Quit();
        }
        else 
            return MENU_CONTINUE;
     ***/
    return 1;
  }

  TIM_Cmd( TIM2, DISABLE );

  azimuth = Sound_GetAzimuth();
  if(oldAzimuth != azimuth)
    soundCounter = 2;
  oldAzimuth = azimuth;
  soundCounter = 2;
  if( (_Sound_BufferStatus & LOW_EMPTY ) && soundCounter) {
#ifndef SOUND_WINDOWS_FUNCTION_ENABLED
    i = Sound_ReadWavFile_Stereo(fileInfo, _Sound_StereoWavBuffer, 2048);
    Sound_Position(_Sound_AudioBuffer, _Sound_StereoWavBuffer, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);

    if ( i < half_audio_buffer ) {
      _Sound_fEOF  = 1;
    }
#else
    STOPWATCH_START
    Sound_FillBufferWF(fileInfo, azimuth, 0);
    STOPWATCH_STOP
    //CycleCounter_Print(0, 130, 10);
#endif
    _Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(LOW_EMPTY);
    soundCounter--;
  }

  if( (_Sound_BufferStatus & HIGH_EMPTY )  && soundCounter){
#ifndef SOUND_WINDOWS_FUNCTION_ENABLED
    i = Sound_ReadWavFile_Stereo(fileInfo, _Sound_StereoWavBuffer, 2048);
    Sound_Position(_Sound_AudioBuffer+half_audio_buffer, _Sound_StereoWavBuffer, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);

    if ( i< half_audio_buffer ) {
      _Sound_fEOF  = 1;
    }
#else
    STOPWATCH_START
    Sound_FillBufferWF(fileInfo, azimuth, 1);
    STOPWATCH_STOP
    //CycleCounter_Print(0, 130, 10);
#endif
    _Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(HIGH_EMPTY);
    soundCounter--;
  }

  TIM_Cmd( TIM2, ENABLE );

  return 0;
}
#endif

int32_t Sound_ToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length) {
  int i;
  
  for (i=0; i<length; i++) {
    complexBuffer[2*i] = buffer[i];
    complexBuffer[2*i+1] = 0;
  }   
  return length;
}

int32_t Sound_HRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, HRTF_StereoSignal *stereoSignal, int32_t length) {
  int i;

  for (i=0; i<length; i++) {
    complexBuffer[2*i] = stereoSignal->left[i][0];
    complexBuffer[2*i+1] = stereoSignal->right[i][0];
  }
  return length;
}

void Sound_WindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
#ifdef FIXED_POINT
    int32_t value;
    value = buffer1[i]*_Sound_HannWindowsFunction[i];
    buffer2[i] = value>>15; 
#else
    buffer2[i] = buffer1[i]*_Sound_HannWindowsFunction[i];
#endif
  }
}

void Sound_CleanBuffer(NUMBER_TYPE *buffer, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer[i] = 0;
  }
}

void Sound_CopyToAudioBuffer(SOUND_BUFFER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
#ifdef FIXED_POINT
  int32_t sig;
#else
  float32_t sig;
#endif

  for (i=0; i<length; i++) {
    sig = buffer1[i]*SOUND_FFT_SCALE;
    if(sig>MAX_SIGNAL)
      sig = MAX_SIGNAL;
    else
      if(sig<MIN_SIGNAL)
        sig = MIN_SIGNAL;
#ifdef FIXED_POINT
    buffer2[i] = (SOUND_BUFFER_TYPE) sig;
#else
    buffer2[i] = (SOUND_BUFFER_TYPE) roundf(sig);
#endif
  }
}

void Sound_AddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
#ifdef FIXED_POINT
    int32_t value;
    value = buffer2[i] + buffer1[i];
    if(value>NUMBER_AMP_MAX)
      buffer2[i] = NUMBER_AMP_MAX;
    else if(value<NUMBER_AMP_MIN)
      buffer2[i] = NUMBER_AMP_MIN;
    else
      buffer2[i] = value;
#else
    buffer2[i] = buffer2[i] + buffer1[i];
#endif
  }
}
/*
void Sound_FillBufferWF(FILEINFO *fileInfo, float azimuth, int16_t bufferPosition) {
  NUMBER_TYPE buffer1[SOUND_BUFFER_SAMPLE_LENGTH],
    buffer2[DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer1[2*DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer2[2*DSP_FFT_SAMPLE_LENGTH];
  uint32_t length, readLength;

  length = 2*SOUND_BUFFER_SAMPLE_LENGTH;
  readLength = Sound_ReadWavFile_Mono_Offset(fileInfo, buffer1, length, 2*_Sound_FileOffset);

  Sound_WindowsFunction(buffer2, buffer1, DSP_FFT_SAMPLE_LENGTH);
  Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
  HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
  Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);

  if(bufferPosition==0) {
    Sound_CleanBuffer(_Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(_Sound_DoubleBuffer, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_WindowsFunction(buffer2, buffer1+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(_Sound_DoubleBuffer+DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_CopyToAudioBuffer(_Sound_AudioBuffer, _Sound_DoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
  }
  else {
    Sound_CleanBuffer(_Sound_DoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(_Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_WindowsFunction(buffer2, buffer1+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(_Sound_DoubleBuffer+3*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(_Sound_DoubleBuffer, complexBuffer2+DSP_FFT_SAMPLE_LENGTH, DSP_FFT_SAMPLE_LENGTH);
    Sound_CopyToAudioBuffer(_Sound_AudioBuffer+2*DSP_FFT_SAMPLE_LENGTH, _Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
  }

  if(readLength != length)
    _Sound_FileOffset = 0;
  else
    _Sound_FileOffset += DSP_FFT_SAMPLE_LENGTH;
}
*/
#if 0
void Sound_FillBuffer3D(int16_t *readPtr, float azimuth, int16_t bufferPosition) {
  NUMBER_TYPE *buffer1,
  buffer2[DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer1[2*DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer2[2*DSP_FFT_SAMPLE_LENGTH];
  /*
  uint32_t length, readLength;
  
  length = 2*SOUND_BUFFER_SAMPLE_LENGTH;
  readLength = Sound_ReadWavFile_Mono_Offset(fileInfo, buffer1, length, 2*_Sound_FileOffset);
  */
  //printf("Sound_FillBuffer3D 1\r\n");
  //buffer1 = (NUMBER_TYPE *)readPtr;
  //printf("Sound_FillBuffer3D readPtr:%d\r\n", readPtr[0]);
  Sound_WindowsFunction(buffer2, readPtr, DSP_FFT_SAMPLE_LENGTH);
  //printf("Sound_FillBuffer3D 1.1\r\n");

  Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
  //printf("Sound_FillBuffer3D 1.2\r\n");

  HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
  //printf("Sound_FillBuffer3D 1.3\r\n");

  Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);
  
  //printf("Sound_FillBuffer3D 2: %d\r\n", bufferPosition);
  
  if(bufferPosition==0) {
    Sound_CleanBuffer(_Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 3\r\n");
    Sound_AddBuffer(_Sound_DoubleBuffer, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 4\r\n");
    Sound_WindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 5\r\n");
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 6\r\n");
    HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    //printf("Sound_FillBuffer3D 7\r\n");
    Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 8\r\n");
    Sound_AddBuffer(_Sound_DoubleBuffer+DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 9\r\n");
    Sound_CopyToAudioBuffer(_Sound_AudioBuffer, _Sound_DoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D _Sound_AudioBuffer:%x, %d\r\n", _Sound_AudioBuffer, _Sound_AudioBuffer[0]);
  }
  else {
    Sound_CleanBuffer(_Sound_DoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 11\r\n");
    Sound_AddBuffer(_Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 12\r\n");
    Sound_WindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 13\r\n");
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 14\r\n");
    HRTF_SoundPosition(&_Sound_StereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    //printf("Sound_FillBuffer3D 15\r\n");
    Sound_HRTFToComplexBuffer(complexBuffer2, &_Sound_StereoSignal, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 16\r\n");
    Sound_AddBuffer(_Sound_DoubleBuffer+3*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 17\r\n");
    Sound_AddBuffer(_Sound_DoubleBuffer, complexBuffer2+DSP_FFT_SAMPLE_LENGTH, DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D 18\r\n");
    Sound_CopyToAudioBuffer(_Sound_AudioBuffer+2*DSP_FFT_SAMPLE_LENGTH, _Sound_DoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
    //printf("Sound_FillBuffer3D _Sound_AudioBuffer:%x, %d\r\n", &_Sound_AudioBuffer[2*DSP_FFT_SAMPLE_LENGTH], _Sound_AudioBuffer[2*DSP_FFT_SAMPLE_LENGTH]);
  }
  /*
  if(readLength != length)
    _Sound_FileOffset = 0;
  else
    _Sound_FileOffset += DSP_FFT_SAMPLE_LENGTH;
   */
}
#endif

#if 0
int32_t Sound_ReadWavFile_Stereo(FILEINFO *fileInfo, NUMBER_TYPE *soundBuffer, uint32_t length) {
  uint32_t i, j, halfLength;
  NUMBER_TYPE monoWavBuffer[2*DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE *buffer;

  cwFSReadFile(fileInfo, monoWavBuffer, &i, length);
  buffer = monoWavBuffer;
  halfLength = i>>1;
  for(j=0; j<halfLength; j++) {
    soundBuffer[4*j] = buffer[2*j];
    soundBuffer[4*j+1] = buffer[2*j+1];
    soundBuffer[4*j+2] = 0;
    soundBuffer[4*j+3] = 0;
  }
  if(i==0) {
    cwFSSeek(fileInfo, HEADER_SIZE);
    cwFSReadFile(fileInfo, monoWavBuffer, &i, length);
    buffer = monoWavBuffer;
    halfLength = i>>1;
    for(j=0; j<halfLength; j++) {
      soundBuffer[4*j] = buffer[2*j];
      soundBuffer[4*j+1] = buffer[2*j+1];
      soundBuffer[4*j+2] = 0;
      soundBuffer[4*j+3] = 0;
    }
  }
  else if(i<length) {
    for(j=i; j<length; j++) {
      soundBuffer[j] = 0;
    }
    cwFSSeek(fileInfo, HEADER_SIZE);
  }
  return length;
}

int32_t Sound_ReadWavFile_Mono_Offset(FILEINFO *fileInfo, NUMBER_TYPE *soundBuffer, uint32_t length, uint32_t offset) {
  uint32_t readLength;

  cwFSSeek(fileInfo, HEADER_SIZE+offset);
  cwFSReadFile(fileInfo, soundBuffer, &readLength, length);
  return readLength;
}
#endif
