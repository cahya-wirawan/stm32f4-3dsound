/*
*
* File Name          :  ClockCounter.c
* Description        :  Clock Counter.
*/

#include "ClockCounter.h"
#include <stdio.h>

#define CC_MAX_STRING_LENGTH 15

int _CycleCounter_Cycle[2];
//static uint8_t _CycleCounter_String[CC_MAX_STRING_LENGTH+1];
volatile unsigned int *DWT_CYCCNT     = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL    = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *SCB_DEMCR      = (volatile unsigned int *)0xE000EDFC; //address of the register

/*******************************************************************************
* Function Name  : Cycle_Counter_Init
* Description    : Initialization function for counting the cycles
* Input          : None
* Return         : None
*******************************************************************************/
void CycleCounter_Init(void) {
    *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT = 0; // reset the counter
    *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

/*******************************************************************************
* Function Name  : Cycle_Counter_Print
* Description    : Print cycles
* Input          : x, y
* Return         : None
*******************************************************************************/
void CycleCounter_Print(uint8_t x, uint8_t y, uint8_t length) {
	printf("Number of cycles: %d\n", _CycleCounter_Cycle[1]);
}
