#include <stm32f10x.h>

#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_exti.h>
#include <misc.h>


#include "standby.h"
#include "bsp.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>

//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//printf("Sys_Enter_Standby ...  \r\n");

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF,ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,DISABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,DISABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC,DISABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,DISABLE);
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF,DISABLE);

	GPIO_ResetBits(GPIOA,GPIO_Pin_All);
	GPIO_ResetBits(GPIOB,GPIO_Pin_All);
	GPIO_ResetBits(GPIOC,GPIO_Pin_All);
	GPIO_ResetBits(GPIOD,GPIO_Pin_All);
	GPIO_ResetBits(GPIOF,GPIO_Pin_All);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,DISABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//ʹ��PWR����ʱ��
	PWR_WakeUpPinCmd(ENABLE);  //ʹ�ܻ��ѹܽŹ���
	PWR_EnterSTANDBYMode();	  //���������STANDBY��ģʽ 	
	
}

void Sys_Enter_DeepStandby(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//printf("Sys_Enter_Standby ...  \r\n");

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF,ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//ʹ��PWR����ʱ��
	PWR_WakeUpPinCmd(DISABLE);
	PWR_EnterSTANDBYMode();	  //���������STANDBY��ģʽ
	
}


void EXTI0_IRQHandler(void)
{
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
	{
		
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}


//PA0 WKUP���ѳ�ʼ��
void WKUP_Pin_Init(void)
{	
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;  

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 						 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

