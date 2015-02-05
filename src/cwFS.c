/*
 *  Fs.c
 *
 *  Author: Cahya Wirawan
 *
 */
#include <stdio.h>
#include <stdint.h>
#include "cwFS.h"
#include "Number.h"
#include "DSP.h"
#include "cwWave.h"

uint32_t cwFSReadFile(FILEINFO *fileInfo, NUMBER_TYPE *buffer, uint32_t *successcount, uint32_t length)
{
  uint32_t i, readLength;
  int16_t localBuffer[2*DSP_FFT_SAMPLE_LENGTH];

  if(length>4*DSP_FFT_SAMPLE_LENGTH)
    return 0;
  f_read(fileInfo, (void *)localBuffer, (UINT) length, (UINT *) successcount);
  readLength = (*successcount)>>1;
  for(i=0; i<readLength; i++){
    buffer[i] = localBuffer[i];
  }
  return 0;
}

void cwFSSeek(FILEINFO *fileInfo,  uint32_t offset)
{
  f_lseek(fileInfo, offset);
}

#if 0
uint8_t cwFSScanDir (char* path, char fileList[][CW_WAVE_MAX_FILENAME])
{
  FRESULT res;
  FILINFO fno;
  DIR dir;
  uint8_t fileCounter;
  char *fn;

  fileCounter = 0;
  res = f_opendir(&dir, path);                       /* Open the directory */
  printf("result %d", res);
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
      fn = fno.fname;
      if (fno.fattrib & AM_DIR) {                    /* It is a directory */
        continue;
      } else {                                       /* It is a file. */
        printf("%s/%s\n", path, fn);
        sprintf(fileList[fileCounter], "%s/%s", path, fn);
        fileCounter++;
        if(fileCounter==CW_WAVE_MAX_FILELIST)
          break;
      }
    }
  }

  return fileCounter;
}
#endif
