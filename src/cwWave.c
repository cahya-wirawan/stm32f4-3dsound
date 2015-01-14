#include "cwWave.h"
#include "cwFS.h"
#include "main.h"
#include "Audio.h"
#include "Sound.h"
#include "ClockCounter.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"


CWWaveFormatTypeDef cwWaveFormat;
static uint32_t speechDataOffset;
static uint32_t waveCounter;
int16_t *audioBuffer;
uint8_t cwBufferSwitch = 1;
UINT cwBytesRead;
__IO uint32_t cwWaveDataLength = 0;
CWWAVEErrorCode cwWaveFileStatus = Unvalid_RIFF_ID;

extern FATFS fatfs;
extern FIL file;
extern USB_OTG_CORE_HANDLE USB_OTG_Core;
FIL cwFileHandle;

char cwWaveSoundDir[] = "0:/sound";
char cwWaveFileList[CW_FS_MAX_FILELIST][CW_FS_MAX_FILENAME];
uint8_t cwWaveFileCounter;
uint8_t cwWaveCurrentFileNumber;
uint8_t volume = 70, AudioPlayStart = 0;
uint32_t WaveCounter;
uint8_t RepeatState = 0;
extern uint32_t AudioRemSize;

static CWWAVEErrorCode cwWaveParse(uint8_t *header)
{
  uint32_t temp = 0x00;
  uint32_t extraformatbytes = 0;
  //uint16_t buffer[WAV_BUFFER_SIZE] ={0x00};
  
  /* Read chunkID, must be 'RIFF' */
  temp = cwWaveReadUnit((uint8_t*)header, 0, 4, BigEndian);
  if (temp != CHUNK_ID)
  {
    return(Unvalid_RIFF_ID);
  }
  
  /* Read the file length */
  cwWaveFormat.RIFFchunksize = cwWaveReadUnit((uint8_t*)header, 4, 4, LittleEndian);
  
  /* Read the file format, must be 'WAVE' */
  temp = cwWaveReadUnit((uint8_t*)header, 8, 4, BigEndian);
  if (temp != FILE_FORMAT)
  {
    return(Unvalid_WAVE_Format);
  }
  
  /* Read the format chunk, must be'fmt ' */
  temp = cwWaveReadUnit((uint8_t*)header, 12, 4, BigEndian);
  if (temp != FORMAT_ID)
  {
    return(Unvalid_FormatChunk_ID);
  }
  /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
  temp = cwWaveReadUnit((uint8_t*)header, 16, 4, LittleEndian);
  if (temp != 0x10)
  {
    extraformatbytes = 1;
  }
  /* Read the audio format, must be 0x01 (PCM) */
  cwWaveFormat.FormatTag = cwWaveReadUnit((uint8_t*)header, 20, 2, LittleEndian);
  if (cwWaveFormat.FormatTag != WAVE_FORMAT_PCM)
  {
    return(Unsupporetd_FormatTag);
  }
  
  /* Read the number of channels, must be 0x01 (Mono) or 0x02 (Stereo) */
  cwWaveFormat.NumChannels = cwWaveReadUnit((uint8_t*)header, 22, 2, LittleEndian);
  
  /* Read the Sample Rate */
  cwWaveFormat.SampleRate = cwWaveReadUnit((uint8_t*)header, 24, 4, LittleEndian);
  
  /* Read the Byte Rate */
  cwWaveFormat.ByteRate = cwWaveReadUnit((uint8_t*)header, 28, 4, LittleEndian);
  
  /* Read the block alignment */
  cwWaveFormat.BlockAlign = cwWaveReadUnit((uint8_t*)header, 32, 2, LittleEndian);
  
  /* Read the number of bits per sample */
  cwWaveFormat.BitsPerSample = cwWaveReadUnit((uint8_t*)header, 34, 2, LittleEndian);
  if (cwWaveFormat.BitsPerSample != BITS_PER_SAMPLE_16)
  {
    return(Unsupporetd_Bits_Per_Sample);
  }
  speechDataOffset = 36;
  /* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" */
  if (extraformatbytes == 1)
  {
    /* Read th Extra format bytes, must be 0x00 */
    temp = cwWaveReadUnit((uint8_t*)header, 36, 2, LittleEndian);
    if (temp != 0x00)
    {
      return(Unsupporetd_ExtraFormatBytes);
    }
    /* Read the Fact chunk, must be 'fact' */
    temp = cwWaveReadUnit((uint8_t*)header, 38, 4, BigEndian);
    if (temp != FACT_ID)
    {
      return(Unvalid_FactChunk_ID);
    }
    /* Read Fact chunk data Size */
    temp = cwWaveReadUnit((uint8_t*)header, 42, 4, LittleEndian);
    
    speechDataOffset += 10 + temp;
  }
  /* Read the Data chunk, must be 'data' */
  temp = cwWaveReadUnit((uint8_t*)header, speechDataOffset, 4, BigEndian);
  speechDataOffset += 4;
  if (temp != DATA_ID)
  {
    return(Unvalid_DataChunk_ID);
  }
  
  /* Read the number of sample data */
  cwWaveFormat.DataSize = cwWaveReadUnit((uint8_t*)header, speechDataOffset, 4, LittleEndian);
  speechDataOffset += 4;
  waveCounter =  speechDataOffset;
  return(Valid_WAVE_File);
}


uint32_t cwWaveReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat)
{
  uint32_t index = 0;
  uint32_t temp = 0;
  
  for (index = 0; index < NbrOfBytes; index++)
  {
    temp |= buffer[idx + index] << (index * 8);
  }
  
  if (BytesFormat == BigEndian)
  {
    temp = __REV(temp);
  }
  return temp;
}

void WavePlayerStart(void)
{
  DIR dir;
  cwWaveFileCounter = cwFSScanDir(cwWaveSoundDir, cwWaveFileList);
  
  cwBufferSwitch = 1;
  
  /* Get the read out protection status */
  if (f_opendir(&dir, cwWaveSoundDir)!= FR_OK)
  {
    while(1)
    {
      TM_DISCO_LedToggle(LED_RED);
      Delayms(1000);
    }
  }
  else
  {
    cwWaveOpenNextFile();
    WavePlayBack(I2S_AudioFreq_44k);
  }
}

void cwWaveOpenNextFile(void)
{
  uint8_t i;
  FRESULT fresult;
  uint8_t wavHeader[_MAX_SS];
  
  f_close(&cwFileHandle);
  for(i = 0; i < cwWaveFileCounter; i++){
    fresult = f_open(&cwFileHandle, cwWaveFileList[cwWaveCurrentFileNumber], FA_READ);
    if (fresult != FR_OK) {
      cwWaveCurrentFileNumber = (cwWaveCurrentFileNumber+1)%cwWaveFileCounter;
      continue;
    }
    else {
      f_read (&cwFileHandle, (void *) wavHeader, _MAX_SS, &cwBytesRead);
      cwWaveFileStatus = cwWaveParse((uint8_t *) wavHeader);
      if (cwWaveFileStatus == Valid_WAVE_File) {
        cwWaveDataLength = cwWaveFormat.DataSize;
        printf("Reading sound file %s\n", cwWaveFileList[cwWaveCurrentFileNumber]);
        cwWaveCurrentFileNumber = (cwWaveCurrentFileNumber+1)%cwWaveFileCounter;
        break;
      }
      else {
        f_close(&cwFileHandle);
        cwWaveCurrentFileNumber = (cwWaveCurrentFileNumber+1)%cwWaveFileCounter;
        continue;
      }
    }
  }
  if(i == cwWaveFileCounter){
    /* Led Red Toggles in infinite loop */
    while(1)
    {
      TM_DISCO_LedToggle(LED_RED);
      Delayms(1000);
    }
  }
}

void WavePlayBack(uint32_t AudioFreq)
{  
  printf("WavePlayBack start\n");
  /* Start playing */
  
  audioBuffer = Sound_Get_AudioBuffer();
  Sound_Init();
  AudioPlayStart = 1;
  RepeatState =0;
  
  /* Initialize wave player (Codec, DMA, I2C) */
  WavePlayerInit(AudioFreq);
  AudioRemSize   = 0;
  
  STOPWATCH_START
  /* Get Data from USB Key */
  f_lseek(&cwFileHandle, WaveCounter);
  //f_read (&cwFileHandle, audioBuffer, _MAX_SS, &BytesRead);
  STOPWATCH_STOP
  CycleCounter_Print(0,0,0);
  
  /* Start playing wave */
  Audio_MAL_Play((uint32_t)audioBuffer, 2*_MAX_SS);
  
  while(HCD_IsDeviceConnected(&USB_OTG_Core))
  {
    /* Test on the command: Playing */
    if (1)
    {
      
      /* wait for DMA transfer complete */
      while((AUDIO_PlaybackBuffer_GetStatus(0) == FULL) &&  HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        cwWaveOpenNextFile();
        f_lseek(&cwFileHandle, WaveCounter);
        Sound_Init();
      }
      
      //#define SOUND_DIRECT_TEST
#ifndef	SOUND_DIRECT_TEST
      Sound_FillBuffer(&cwFileHandle);
#else
      Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(0);
      if(Sound_BufferStatus & LOW_EMPTY)
      {
        f_read (&cwFileHandle, audioBuffer, _MAX_SS, &BytesRead);
        Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(LOW_EMPTY);
      }
      if(Sound_BufferStatus & HIGH_EMPTY)
      {
        f_read (&cwFileHandle, audioBuffer+_MAX_SS/2, _MAX_SS, &BytesRead);
        Sound_BufferStatus = AUDIO_PlaybackBuffer_GetStatus(HIGH_EMPTY);
      }
#endif
    }
    else
    {
      WavePlayerStop();
      cwWaveDataLength = 0;
      RepeatState =0;
      break;
    }
    if(cwWaveDataLength == 0) {
      cwWaveDataLength = cwWaveFormat.DataSize;
      f_lseek(&cwFileHandle, 44);
    }
  }
  
  AudioPlayStart = 0;
  WavePlayerStop();
}

/**
 * @brief  Pause or Resume a played wave
 * @param  state: if it is equal to 0 pause Playing else resume playing
 * @retval None
 */
void WavePlayerPauseResume(uint8_t state)
{
  EVAL_AUDIO_PauseResume(state);
}

/**
 * @brief  Configure the volune
 * @param  vol: volume value
 * @retval None
 */
uint8_t WaveplayerCtrlVolume(uint8_t vol)
{
  EVAL_AUDIO_VolumeCtl(vol);
  return 0;
}


/**
 * @brief  Stop playing wave
 * @param  None
 * @retval None
 */
void WavePlayerStop(void)
{
  EVAL_AUDIO_Stop(CODEC_PDWN_SW);
}

/**
 * @brief  Initializes the wave player
 * @param  AudioFreq: Audio sampling frequency
 * @retval None
 */
int WavePlayerInit(uint32_t AudioFreq)
{
  
  /* Initialize I2S interface */
  EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
  
  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
  EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, volume, AudioFreq );
  
  return 0;
}

/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{
  //printf("0,");
#ifdef AUDIO_MAL_MODE_CIRCULAR
  if (cwWaveDataLength) cwWaveDataLength -= _MAX_SS;
  if (cwWaveDataLength < _MAX_SS) cwWaveDataLength = 0;
  AUDIO_PlaybackBuffer_SetStatus(LOW_EMPTY);
#endif /* AUDIO_MAL_MODE_CIRCULAR */
  
  /* Generally this interrupt routine is used to load the buffer when
   a streaming scheme is used: When first Half buffer is already transferred load
   the new data to the first half of buffer while DMA is transferring data from
   the second half. And when Transfer complete occurs, load the second half of
   the buffer while the DMA is transferring from the first half ... */
  /*
   ...........
   */
}

/**
 * @brief  Calculates the remaining file size and new position of the pointer.
 * @param  None
 * @retval None
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
  /* Calculate the remaining audio data in the file and the new size
   for the DMA transfer. If the Audio files size is less than the DMA max
   data transfer size, so there is no calculation to be done, just restart
   from the beginning of the file ... */
  /* Check if the end of file has been reached */
  static uint16_t counter=0;
  if(counter%10000 == 0)
    printf("EVAL_AUDIO_TransferComplete_CallBack\r\n");
  counter++;
  counter%=10000;
  if (cwWaveDataLength) cwWaveDataLength -= _MAX_SS;
  if (cwWaveDataLength < _MAX_SS) cwWaveDataLength = 0;
  AUDIO_PlaybackBuffer_SetStatus(HIGH_EMPTY);
}

/**
 * @brief  Manages the DMA FIFO error interrupt.
 * @param  None
 * @retval None
 */
void EVAL_AUDIO_Error_CallBack(void* pData)
{
  /* Stop the program with an infinite loop */
  while (1)
  {}
  
  /* could also generate a system reset to recover from the error */
  /* .... */
}

/**
 * @brief  Get next data sample callback
 * @param  None
 * @retval Next data sample to be sent
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  return 0;
}


/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t Codec_TIMEOUT_UserCallback(void)
{
  return (0);
}