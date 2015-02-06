//
//  cwSound3D.c
//  

#include "arm_math.h"
#include "string.h"
#include "Audio.h"
#include "cwSound3D.h"

#define MAX_SIGNAL 32767.0f
#define MIN_SIGNAL -MAX_SIGNAL
#define SOUND_FFT_SCALE 0.00001f

NUMBER_TYPE cwSound3DStereoWavBuffer[SOUND_BUFFER_LENGTH];
HRTF_StereoSignal cwSound3DStereoSignal;
NUMBER_TYPE cwSound3DDoubleBuffer[4*DSP_FFT_SAMPLE_LENGTH];

const NUMBER_TYPE cwSound3DHannWindowsFunction [] =
{
#include "WindowsFunction.h"
};

int16_t cwSound3DInit(void) {
  int i;
  
  for (i=0; i<4*DSP_FFT_SAMPLE_LENGTH; i++) {
    cwSound3DDoubleBuffer[i] = 0;
  }
  HRTF_Init();
  return 1;
}

void cwSound3DFillBuffer(int16_t * audioBuffer, int16_t *readPtr, float azimuth, int16_t bufferNumber) {
  NUMBER_TYPE buffer2[DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer1[2*DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer2[2*DSP_FFT_SAMPLE_LENGTH];

  cwSound3DWindowsFunction(buffer2, readPtr, DSP_FFT_SAMPLE_LENGTH);
  
  cwSound3DToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
  
  HRTF_SoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
  
  cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
  
  if(bufferNumber==0) {
    cwSound3DCleanBuffer(cwSound3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    cwSound3DWindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    cwSound3DCopyToAudioBuffer(audioBuffer, cwSound3DDoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
  }
  else {
    cwSound3DCleanBuffer(cwSound3DDoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    cwSound3DWindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&cwSound3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    cwSound3DHRTFToComplexBuffer(complexBuffer2, &cwSound3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer+3*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DAddBuffer(cwSound3DDoubleBuffer, complexBuffer2+DSP_FFT_SAMPLE_LENGTH, DSP_FFT_SAMPLE_LENGTH);
    cwSound3DCopyToAudioBuffer(audioBuffer, cwSound3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
  }
}

void cwSound3DWindowsFunction(NUMBER_TYPE *buffer2, int16_t *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer2[i] = buffer1[i]*cwSound3DHannWindowsFunction[i];
  }
}

int32_t cwSound3DToComplexBuffer(NUMBER_TYPE *complexBuffer, NUMBER_TYPE *buffer, int32_t length) {
  int i;
  
  for (i=0; i<length; i++) {
    complexBuffer[2*i] = buffer[i];
    complexBuffer[2*i+1] = 0;
  }
  return length;
}

int32_t cwSound3DHRTFToComplexBuffer(NUMBER_TYPE *complexBuffer, HRTF_StereoSignal *stereoSignal, int32_t length) {
  int i;
  
  for (i=0; i<length; i++) {
    complexBuffer[2*i] = stereoSignal->left[i][0];
    complexBuffer[2*i+1] = stereoSignal->right[i][0];
  }
  return length;
}

void cwSound3DCleanBuffer(NUMBER_TYPE *buffer, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer[i] = 0;
  }
}

void cwSound3DCopyToAudioBuffer(SOUND_BUFFER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
  float32_t sig;
  
  for (i=0; i<length; i++) {
    sig = buffer1[i]*SOUND_FFT_SCALE;
    if(sig>MAX_SIGNAL)
      sig = MAX_SIGNAL;
    else
      if(sig<MIN_SIGNAL)
        sig = MIN_SIGNAL;
    buffer2[i] = (SOUND_BUFFER_TYPE) roundf(sig);
  }
}

void cwSound3DAddBuffer(NUMBER_TYPE *buffer2, NUMBER_TYPE *buffer1, int32_t length) {
  int32_t i;
  
  for (i=0; i<length; i++) {
    buffer2[i] = buffer2[i] + buffer1[i];
  }
}
