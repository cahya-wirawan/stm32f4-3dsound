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
 * @file      cwSoundFile.h
 * @brief     Mems macros and structures
 * @details
 */

#ifndef __CW_SOUND_FILE__
#define __CW_SOUND_FILE__

#include <stdio.h>
#include "Audio.h"
#include "ff.h"

#define   CW_FS_FILE_READ_BUFFER_SIZE 8192

const char *cwSFGetFilenameExt(const char *filename);
FRESULT cwSFPlayDirectory (const char* path, unsigned char seek);

#endif /* defined(__CW_SOUND_FILE__) */
