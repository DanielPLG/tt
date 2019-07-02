
#ifndef _SYSTICK_H
#define _SYSTICK_H

//#include "stm32f10x_type.h"

void SysTick_Config1(void);
//void Delay_Ms(u32 nTime);
void TimingDelay_Decrement(void);
void Start_Count_Click(void);
void Stop_Count_Click(void);

#endif /* _SYSTICK_H */

