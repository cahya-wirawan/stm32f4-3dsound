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
int16_t *cwS3DAudioBuffer;
uint32_t cwWaveDataLength = 0;

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
    cwS3DAudioBuffer = Sound_Get_AudioBuffer();
    Sound_Init();

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
      //printf("cwSFBytesLeft : %d\r\n", cwSFBytesLeft);
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
        cwSFBytesLeft = CW_FS_FILE_READ_BUFFER_SIZE;
        
        // Out of data or error or user button... Stop playback!
        if (br < btr || res != FR_OK || TM_DISCO_ButtonPressed()) {
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
 * CODEC using DMA). One mp3 frame is decoded at a time and
 * provided to the audio driver.
 */
void cwS3DAudioCallback(void *context, int buffer) {
  int16_t *readPtr;
  int byteSent;
  int i;
  
  int outOfData = 0;
  
  //printf("cwS3DAudioCallback 1\r\n");
  
  int16_t *samples;
  if (buffer==0) {
    //printf("cwS3DAudioCallback 1.5 buffer:%d\r\n", buffer);
    samples = cwS3DAudioBuffer;
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
  } else {
    //printf("cwS3DAudioCallback 1.5 buffer:%d\r\n", buffer);
    samples = cwS3DAudioBuffer + SOUND_BUFFER_LENGTH/2;
    TM_DISCO_LedOff(LED_RED);
    TM_DISCO_LedOn(LED_GREEN);
  }
  //printf("cwSFReadPtr : %x\r\n", cwSFReadPtr);
  readPtr = (int16_t*)cwSFReadPtr;
#if 0
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
  //printf("cwS3DAudioCallback 3.1\r\n");
#else
  //printf("cwS3DAudioCallback 4\r\n");
  float azimuth = cwMemsGetAzimuth();
  //int iAzimuth = (int) azimuth;
  //printf("azimuth Y: %d\r\n", iAzimuth);
  if (buffer==0) {
    //printf("cwS3DAudioCallback 5\r\n");
    Sound_FillBuffer3D(readPtr, azimuth, 0);
  }
  else {
    //printf("cwS3DAudioCallback 6\r\n");
    Sound_FillBuffer3D(readPtr, azimuth, 1);
  }
  
  byteSent = CW_WAVE_BYTES_TO_SEND;
  cwSFBytesLeft -= CW_WAVE_BYTES_TO_SEND;
  cwSFReadPtr += CW_WAVE_BYTES_TO_SEND;

#endif
  if (!outOfData) {
    //printf("cwS3DAudioCallback 7, byteSent: %d, addr: %x, samples[0]=%d\r\n", byteSent, &samples[0], samples[0]);
    ProvideAudioBuffer(samples, byteSent);
    //printf("cwS3DAudioCallback 8\r\n");
  }
}
