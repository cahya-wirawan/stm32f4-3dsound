/*
 * Audio.h
 *
 *  Created on: Apr 25, 2012
 *      Author: Cahya Wirawan
 */

#ifndef AUDIO_H_
#define AUDIO_H_

typedef enum {
	FULL = 0,
	LOW_EMPTY = 1,
	HIGH_EMPTY = 2
} AUDIO_PlaybackBuffer_Status;

AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_GetStatus( AUDIO_PlaybackBuffer_Status value );
AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_SetStatus( AUDIO_PlaybackBuffer_Status value );

#endif /* AUDIO_H_ */
