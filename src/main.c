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
 * @file      main.c
 * @brief     3D Sound effect on stm32f4 discovery board
 * @details
 * @author		Cahya Wirawan
 * @email		  cahya.wirawan at gmail.com
 * @website		http://cahya-wirawan.github.io/stm32f4-3dsound/
 * @ide		    just gcc with makefile
 * @stdperiph	STM32F4xx Standard peripheral drivers version 1.4.0 or greater required
 *
 */

#include "string.h"
#include "stdio.h"
#include "main.h"
/* Include core modules */
#include "stm32f4xx.h"
/* STM32f4 libraries created by Tilen Majerle */
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_lis302dl_lis3dsh.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"
#include "cwSoundFile.h"
#include "cwMems.h"
#include "cwHrtfArray.h"

int main(void) {
  hwInit();
  
  while(1) {
    TM_USB_MSCHOST_Process();
    if (TM_USB_MSCHOST_Device() == TM_USB_MSCHOST_Result_Connected) {
      while (1) {
        cwSFPlayDirectory("1:", 0);
      }
    }
  }
}

void hwInit() {
	// Initialize System
	//SystemInit();
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}
  // Initialise delay Systick timer
  TM_DELAY_Init();
	// Initialise leds on board
	TM_DISCO_LedInit();
	//Initialise USART2 at 115200 baud, TX: PA2, RX: PA3
  TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	// Initialize button on board
	TM_DISCO_ButtonInit();
  /* Initialize USB MSC HOST */
  TM_USB_MSCHOST_Init();
    
	/* Detect proper device */
  if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS302DL) {
      /* Turn on GREEN and RED */
      TM_DISCO_LedOn(LED_GREEN | LED_RED);
      /* Initialize LIS302DL */
      TM_LIS302DL_LIS3DSH_Init(TM_LIS302DL_Sensitivity_2_3G, TM_LIS302DL_Filter_2Hz);
  } else if (TM_LIS302DL_LIS3DSH_Detect() == TM_LIS302DL_LIS3DSH_Device_LIS3DSH) {
      /* Turn on BLUE and ORANGE */
      TM_DISCO_LedOn(LED_BLUE | LED_ORANGE);
      /* Initialize LIS3DSH */
      TM_LIS302DL_LIS3DSH_Init(TM_LIS3DSH_Sensitivity_2G, TM_LIS3DSH_Filter_800Hz);
  } else {
      /* Device is not recognized */      
      /* Turn on ALL leds */
      TM_DISCO_LedOn(LED_GREEN | LED_RED | LED_BLUE | LED_ORANGE);
      
      /* Infinite loop */
      while (1);
  }
  cwMemsInit();
  printf("\r\n");
  printf("3D Sound Player (version %s, date %s) by Cahya Wirawan <cahya.wirawan@gmail.com>\r\n",
         CW_APP_VERSION, CW_APP_DATE);
  printf("HRTF Database used: %s \r\n",CW_HRTF_ARRAY_NAME);
  printf("Peripheries initialised\r\n");
  Delayms(1000);
}
