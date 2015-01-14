/*
*
* File Name          :  ClockCounter.h
* Description        :  Clock Counter.
*/

#ifndef CYCLECOUNTER_H_
#define CYCLECOUNTER_H_

#include <stdint.h>

#define STOPWATCH_START { _CycleCounter_Cycle[0] = *DWT_CYCCNT;}
#define STOPWATCH_STOP { _CycleCounter_Cycle[1] = *DWT_CYCCNT; _CycleCounter_Cycle[1] = _CycleCounter_Cycle[1] - _CycleCounter_Cycle[0]; }

extern int _CycleCounter_Cycle[2];
extern volatile unsigned int *DWT_CYCCNT;  //address of the register
extern volatile unsigned int *DWT_CONTROL; //address of the register
extern volatile unsigned int *SCB_DEMCR;   //address of the register

void CycleCounter_Init(void);
void CycleCounter_Print(uint8_t x, uint8_t y, uint8_t length);

#endif //CYCLECOUNTER_H_
