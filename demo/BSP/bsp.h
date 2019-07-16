#ifndef __BSP__
#define __BSP__


#define V33_RCC_TYPE			RCC_APB2PeriphClockCmd
#define V33_RCC					RCC_APB2Periph_GPIOC
#define V33_GPIO				GPIOC
#define V33_PIN					GPIO_Pin_14


void init_uart1(void);
void init_uart2(void);



#endif

