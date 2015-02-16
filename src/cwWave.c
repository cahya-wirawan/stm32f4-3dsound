/*
 STM32F4-3DSound - Copyright (C) 2015 Cahya Wirawan.
 
 This file is part of STM32F4-3DSound
 
 STM32F4-3DSound is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 STM32F4-3DSound is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file      cwWave.c
 * @brief     The wave sound file reader
 * @details
 */

#include "stdio.h"
#include "string.h"
#include "Audio.h"
#include "cwSoundFile.h"
#include "cwSound3D.h"
#include "cwWave.h"
#include "cwMems.h"
#include "tm_stm32f4_disco.h"

#define CW_WAVE_BYTES_TO_SEND 2048


extern FIL cwSFFile;
extern char cwSFFileReadBuffer[CW_FS_FILE_READ_BUFFER_SIZE];
extern volatile int cwSFBytesLeft;
extern char *cwSFReadPtr;

WAVE_FormatTypeDef cwWaveFormat;
uint32_t cwWaveDataLength = 0;

int16_t cwWaveAudioBuffer[SOUND_BUFFER_LENGTH];

void cwWavePlayFile(char* filename) {
  unsigned int br, btr;
  FRESULT res;
  
  cwSFBytesLeft = CW_FS_FILE_READ_BUFFER_SIZE;
  cwSFReadPtr = cwSFFileReadBuffer;
  
  if (FR_OK == f_open(&cwSFFile, filename, FA_OPEN_EXISTING | FA_READ)) {
    res = f_read (&cwSFFile, &cwWaveFormat, sizeof(cwWaveFormat), &br);
    printf("The file %s has following information\r\n", filename);
    printf("File size: %lu\r\n", cwWaveFormat.FileSize);
    printf("Number of channels: %d\r\n", (int16_t) cwWaveFormat.NbrChannels);
    printf("Bit per sample: %d\r\n", (int16_t) cwWaveFormat.BitPerSample);
    printf("Sample rate: %lu\r\n", cwWaveFormat.SampleRate);
    if(!(res == FR_OK && (cwWaveFormat.NbrChannels == 1) && cwWaveFormat.SampleRate == 44100)) {
      printf("The wav file %s needs to have sample rate of 44100Hz and a mono channel\r\n", filename);
      return;
    }
    
    cwWaveDataLength = cwWaveFormat.FileSize;
    cwSound3DInit();
    
    res = f_read(&cwSFFile, cwSFFileReadBuffer, CW_FS_FILE_READ_BUFFER_SIZE, &br);
    InitializeAudio(Audio44100HzSettings);
    SetAudioVolume(0xAF);
    PlayAudioWithCallback(cwWaveAudioCallback, 0);
    
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
          res = f_lseek(&cwSFFile, sizeof(cwWaveFormat));
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
void cwWaveAudioCallback(void *context, int bufferNumber) {
  int16_t *readPtr;
  int byteSent;
  
  int outOfData = 0;
  
  int16_t *audioBuffer;
  if (bufferNumber==0) {
    audioBuffer = cwWaveAudioBuffer;
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
  } else {
    audioBuffer = cwWaveAudioBuffer + SOUND_BUFFER_LENGTH/2;
    TM_DISCO_LedOff(LED_RED);
    TM_DISCO_LedOn(LED_GREEN);
  }
  readPtr = (int16_t*)cwSFReadPtr;
#if 0
  int i;
  if(cwWaveFormat.NbrChannels == 1) {
    // If it is mono sound:
    for (i=0; i<CW_WAVE_BYTES_TO_SEND/2; i++) {
      audioBuffer[2*i] = readPtr[i];
      audioBuffer[2*i+1] = readPtr[i];
    }
    byteSent = CW_WAVE_BYTES_TO_SEND;
    cwSFBytesLeft -= CW_WAVE_BYTES_TO_SEND;
    cwSFReadPtr += CW_WAVE_BYTES_TO_SEND;
  }
#else
  float elevation, azimuth;
  static int counter = 0;
  
  cwMemsGetPosition(&elevation, &azimuth);
  if(counter%50 == 0) {
    printf("elevation: %3.2f, azimuth: %3.2f\r\n", (double)elevation, (double)azimuth);
  }
  counter++;
  counter %= 50;
  cwSound3DFillBuffer(audioBuffer, readPtr, elevation, azimuth, bufferNumber);
  
  byteSent = CW_WAVE_BYTES_TO_SEND;
  cwSFBytesLeft -= CW_WAVE_BYTES_TO_SEND;
  cwSFReadPtr += CW_WAVE_BYTES_TO_SEND;
#endif
  if (!outOfData) {
    ProvideAudioBuffer(audioBuffer, byteSent);
  }
}
