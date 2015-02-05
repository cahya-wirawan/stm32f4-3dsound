//
//  cwSound3D.h
// 
//
//

#ifndef __CW_SOUND3D__
#define __CW_SOUND3D__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "ff.h"

void cwS3DPlayFile(char* filename);
void cwS3DAudioCallback(void *context,int buffer);
void cwS3DFillBuffer(int16_t *readPtr, float azimuth, int16_t bufferPosition);
int16_t cwS3DInit(void);

#endif /* defined(__CW_SOUND3D__) */
