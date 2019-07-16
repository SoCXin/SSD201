
/* Includes ------------------------------------------------------------------*/

#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"   
#include "stm32f10x_iwdg.h" 
#include "stm32f10x_pwr.h" 
#include "stm32f10x_exti.h" 
#include "stm32f10x_adc.h" 
#include "Systime.h"
#include "usart1.h"
#include "IR.h"

extern u8  TxBuf[61];
extern u8  RxBuf[61]; 

u8 Power_ON(void);
void Init(void);
