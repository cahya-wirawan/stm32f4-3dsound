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
 * @file      cwWave.h
 * @brief     Wave macros and structures
 * @details
 */

#ifndef __CW_WAVE__
#define __CW_WAVE__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "ff.h"

typedef struct
{
  uint32_t   ChunkID;       /* 0 */
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16*/
  uint16_t   AudioFormat;   /* 20 */
  uint16_t   NbrChannels;   /* 22 */
  uint32_t   SampleRate;    /* 24 */
  
  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */
  uint16_t   BitPerSample;  /* 34 */
  uint32_t   SubChunk2ID;   /* 36 */
  uint32_t   SubChunk2Size; /* 40 */
  
}WAVE_FormatTypeDef;

void cwWavePlayFile(char* filename);
void cwWaveAudioCallback(void *context,int buffer);

#endif /* defined(__CW_WAVE__) */
