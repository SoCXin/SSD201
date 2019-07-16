#ifndef __USART1_H
#define	__USART1_H

//#include "stm32f10x.h"
//#include <stdio.h>
#include "MAP.h"

void USART1_Config(void);
void NVIC_ConfigurationUSART1(void);
void writeCom1(u8 *p);
u8 writeCom1Char(u8 ch);

#endif /* __USART1_H */

