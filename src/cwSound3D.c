//
//  cwSound3D.c
//  

#include "string.h"
#include "stdio.h"
#include "Audio.h"
#include "cwSoundFile.h"
#include "cwWave.h"
#include "cwMems.h"
#include "cwSound3D.h"
#include "Sound.h"
#include "tm_stm32f4_disco.h"

#define CW_WAVE_BYTES_TO_SEND 2048

extern FIL cwSFFile;
extern char cwSFFileReadBuffer[CW_FS_FILE_READ_BUFFER_SIZE];
extern volatile int cwSFBytesLeft;
extern char *cwSFReadPtr;

WAVE_FormatTypeDef cwS3DFormat;
uint32_t cwWaveDataLength = 0;

int16_t cwS3DAudioBuffer[SOUND_BUFFER_LENGTH];
NUMBER_TYPE cwS3DStereoWavBuffer[SOUND_BUFFER_LENGTH];
HRTF_StereoSignal cwS3DStereoSignal;
NUMBER_TYPE cwS3DDoubleBuffer[4*DSP_FFT_SAMPLE_LENGTH];

void cwS3DPlayFile(char* filename) {
  unsigned int br, btr;
  FRESULT res;
  
  cwSFBytesLeft = CW_FS_FILE_READ_BUFFER_SIZE;
  cwSFReadPtr = cwSFFileReadBuffer;
  
  if (FR_OK == f_open(&cwSFFile, filename, FA_OPEN_EXISTING | FA_READ)) {
    res = f_read (&cwSFFile, &cwS3DFormat, sizeof(cwS3DFormat), &br);
    printf("The file %s has following information\r\n", filename);
    printf("File size: %lu\r\n", cwS3DFormat.FileSize);
    printf("Number of channels: %d\r\n", (int16_t) cwS3DFormat.NbrChannels);
    printf("Bit per sample: %d\r\n", (int16_t) cwS3DFormat.BitPerSample);
    printf("Sample rate: %lu\r\n", cwS3DFormat.SampleRate);
    if(!(res == FR_OK && (cwS3DFormat.NbrChannels == 1 || cwS3DFormat.NbrChannels == 2)
       && cwS3DFormat.SampleRate == 44100)) {
      printf("The file %s doesn't have the required number of channels or sample rate\r\n", filename);
      return;
    }

    cwWaveDataLength = cwS3DFormat.FileSize;
    cwS3DInit();

    res = f_read(&cwSFFile, cwSFFileReadBuffer, CW_FS_FILE_READ_BUFFER_SIZE, &br);
    InitializeAudio(Audio44100HzSettings);
    SetAudioVolume(0xAF);
    PlayAudioWithCallback(cwS3DAudioCallback, 0);
    
    for(;;) {
      /*
       * If past half of buffer, refill...
       *
       * When cwSFBytesLeft changes, the audio callback has just been executed. This
       * means that there should be enough time to copy the end of the buffer
       * to the beginning and update the pointer before the next audio callback.
       * Getting audio callbacks while the next part of the file is read from the
       * file system should not cause problems.
       */
      // Out of data or error or user button... Stop playback!
      if (cwSFBytesLeft < (CW_FS_FILE_READ_BUFFER_SIZE / 2)) {
        // Copy rest of data to beginning of read buffer
        memcpy(cwSFFileReadBuffer, cwSFReadPtr, cwSFBytesLeft);
        
        // Update read pointer for audio sampling
        cwSFReadPtr = cwSFFileReadBuffer;
        
        // Read next part of file
        btr = CW_FS_FILE_READ_BUFFER_SIZE - cwSFBytesLeft;
        res = f_read(&cwSFFile, cwSFFileReadBuffer + cwSFBytesLeft, btr, &br);
        
        // Update the bytes left variable
        cwSFBytesLeft += br;
        
        // End of song: start over from the begining
        if(br < btr) {
          res = f_lseek(&cwSFFile, sizeof(cwS3DFormat));
        }
        // Error or user button... next song!
        if (res != FR_OK || TM_DISCO_ButtonPressed()) {
          StopAudio();
          
          // Re-initialize and set volume to avoid noise
          InitializeAudio(Audio44100HzSettings);
          SetAudioVolume(0);
          
          // Close currently open file
          f_close(&cwSFFile);
          
          // Wait for user button release
          while(TM_DISCO_ButtonPressed()){};
          
          // Return to previous function
          return;
        }
      }
    }
  }
}

/*
 * Called by the audio driver when it is time to provide data to
 * one of the audio buffers (while the other buffer is sent to the
 * CODEC using DMA). One frame is decoded at a time and
 * provided to the audio driver.
 */
void cwS3DAudioCallback(void *context, int bufferPosition) {
  int16_t *readPtr;
  int byteSent;
  
  int outOfData = 0;
  
  int16_t *samples;
  if (bufferPosition==0) {
    samples = cwS3DAudioBuffer;
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
  } else {
    samples = cwS3DAudioBuffer + SOUND_BUFFER_LENGTH/2;
    TM_DISCO_LedOff(LED_RED);
    TM_DISCO_LedOn(LED_GREEN);
  }
  readPtr = (int16_t*)cwSFReadPtr;
#if 0
  int i;
  if(cwS3DFormat.NbrChannels == 1) {
    // If it is mono sound:
    for (i=0; i<CW_WAVE_BYTES_TO_SEND/2; i++) {
      samples[2*i] = readPtr[i];
      samples[2*i+1] = readPtr[i];
    }
    byteSent = CW_WAVE_BYTES_TO_SEND;
    cwSFBytesLeft -= CW_WAVE_BYTES_TO_SEND;
    cwSFReadPtr += CW_WAVE_BYTES_TO_SEND;
  }
#else
  float azimuth = cwMemsGetAzimuth();
  cwS3DFillBuffer(readPtr, azimuth, bufferPosition);
  
  byteSent = CW_WAVE_BYTES_TO_SEND;
  cwSFBytesLeft -= CW_WAVE_BYTES_TO_SEND;
  cwSFReadPtr += CW_WAVE_BYTES_TO_SEND;

#endif
  if (!outOfData) {
    ProvideAudioBuffer(samples, byteSent);
  }
}

/*
 *******
 *******
 */


int16_t cwS3DInit(void) {
  int i;
  
  for (i=0; i<4*DSP_FFT_SAMPLE_LENGTH; i++) {
    cwS3DDoubleBuffer[i] = 0;
  }
  HRTF_Init();
  return 1;
}

void cwS3DFillBuffer(int16_t *readPtr, float azimuth, int16_t bufferPosition) {
  NUMBER_TYPE buffer2[DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer1[2*DSP_FFT_SAMPLE_LENGTH];
  NUMBER_TYPE complexBuffer2[2*DSP_FFT_SAMPLE_LENGTH];

  Sound_WindowsFunction(buffer2, readPtr, DSP_FFT_SAMPLE_LENGTH);
  
  Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
  
  HRTF_SoundPosition(&cwS3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
  
  Sound_HRTFToComplexBuffer(complexBuffer2, &cwS3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
  
  if(bufferPosition==0) {
    Sound_CleanBuffer(cwS3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(cwS3DDoubleBuffer, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_WindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&cwS3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    Sound_HRTFToComplexBuffer(complexBuffer2, &cwS3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(cwS3DDoubleBuffer+DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_CopyToAudioBuffer(cwS3DAudioBuffer, cwS3DDoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
  }
  else {
    Sound_CleanBuffer(cwS3DDoubleBuffer, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(cwS3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, 2*DSP_FFT_SAMPLE_LENGTH);
    Sound_WindowsFunction(buffer2, readPtr+DSP_FFT_SAMPLE_LENGTH/2, DSP_FFT_SAMPLE_LENGTH);
    Sound_ToComplexBuffer(complexBuffer1, buffer2, DSP_FFT_SAMPLE_LENGTH);
    HRTF_SoundPosition(&cwS3DStereoSignal, (FPComplex *) complexBuffer1, DSP_FFT_SAMPLE_LENGTH, 0, azimuth);
    Sound_HRTFToComplexBuffer(complexBuffer2, &cwS3DStereoSignal, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(cwS3DDoubleBuffer+3*DSP_FFT_SAMPLE_LENGTH, complexBuffer2, DSP_FFT_SAMPLE_LENGTH);
    Sound_AddBuffer(cwS3DDoubleBuffer, complexBuffer2+DSP_FFT_SAMPLE_LENGTH, DSP_FFT_SAMPLE_LENGTH);
    Sound_CopyToAudioBuffer(cwS3DAudioBuffer+2*DSP_FFT_SAMPLE_LENGTH, cwS3DDoubleBuffer+2*DSP_FFT_SAMPLE_LENGTH, 2*DSP_FFT_SAMPLE_LENGTH);
  }
}