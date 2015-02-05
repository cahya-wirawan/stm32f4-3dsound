/*
 * Fs.h
 *
 *  Created on: Apr 26, 2012
 *      Author: User
 */

#ifndef CW_FS_H_
#define CW_FS_H_

#include <stdint.h>
#include "Number.h"
#include "DSP.h"
#include "ff.h"

#define FILEINFO FIL
#define CW_FS_MAX_FILELIST 10
#define CW_FS_MAX_FILENAME 30

uint32_t cwFSReadFile(FILEINFO *fileInfo, NUMBER_TYPE *buffer, uint32_t *successcount, uint32_t length);
void cwFSSeek(FILEINFO *fileInfo,  uint32_t offset);
///uint8_t cwFSScanDir (char* path, char fileList[][CW_FS_MAX_FILENAME]);

#endif /* CW_FILESYSTEM_H_ */
