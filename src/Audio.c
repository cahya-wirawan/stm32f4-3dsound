/*
 * Audio.c
 *
 *  Created on: Apr 25, 2012
 *      Author: User
 */

#include "Audio.h"

volatile AUDIO_PlaybackBuffer_Status  audio_buffer_fill = LOW_EMPTY | HIGH_EMPTY ;

AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_GetStatus( AUDIO_PlaybackBuffer_Status value )
{
	if ( value )
		audio_buffer_fill &= ~value;
	return audio_buffer_fill;
}

AUDIO_PlaybackBuffer_Status AUDIO_PlaybackBuffer_SetStatus( AUDIO_PlaybackBuffer_Status value )
{
	audio_buffer_fill |= value;
	return audio_buffer_fill;
}
