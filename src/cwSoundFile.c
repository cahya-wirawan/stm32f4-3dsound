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
 * @file      cwSoundFile.c
 * @brief     The main sound files reader
 * @details
 */

#include "string.h"
#include "cwSoundFile.h"
///#include "cwMP3.h"
#include "cwWave.h"
#include "cwSound3D.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"

#define SOUND3D_ENABLED 1

FIL cwSFFile;
char cwSFFileReadBuffer[CW_FS_FILE_READ_BUFFER_SIZE];
volatile int cwSFBytesLeft;
char *cwSFReadPtr;

const char *cwSFGetFilenameExt(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

FRESULT cwSFPlayDirectory (const char* path, unsigned char seek) {
  FATFS USB_Fs;
  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn; /* This function is assuming non-Unicode cfg. */
  char buffer[200];
#if _USE_LFN
  static char lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
  
  if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
    return 0;
  if (f_mount(&USB_Fs, "1:", 1) != FR_OK)
    return 0;
  
  res = f_opendir(&dir, path); /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno); /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
      if (fno.fname[0] == '.') continue; /* Ignore dot entry */
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      if (fno.fattrib & AM_DIR) { /* It is a directory */
        
      } else { /* It is a file. */
        sprintf(buffer, "%s/%s", path, fn);
        // Skip "seek" number of mp3 files and ignore file with a starting char '.'
        if (seek || !strncasecmp("1:/.", buffer, 4)) {
          printf("Skip file: %s\r\n", buffer);
          seek--;
          continue;
        }
        printf("Play file: %s with 3D Sound:%d\r\n", buffer, SOUND3D_ENABLED);
        if(strcasecmp("wav", cwSFGetFilenameExt(buffer)) == 0) {
          cwWavePlayFile(buffer);
        }
      }
    }
  }
  
  return res;
}
