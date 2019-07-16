#ifndef __USART3_H
#define	__USART3_H

#include "stm32f10x.h"
#include <stdio.h>

void USART3_Config(void);
void NVIC3_Configuration(void);
int fputcStr3(int ch);
void writeCom3(char *p);

#endif

