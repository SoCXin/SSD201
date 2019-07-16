#include "standby.h"
#include <stm32f0xx.h>

#include <stdio.h>
#include <stdlib.h>
#include <stm32f0xx.h>


void Sys_Standby(void)
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//ʹ��PWR����ʱ��
	PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);  //ʹ�ܻ��ѹܽŹ���
	PWR_EnterSTANDBYMode();	  //���������STANDBY��ģʽ 		 
}
//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
                        RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOF , ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
												 
												 
	printf("Sys_Enter_Standby ...  \r\n");
	Sys_Standby();
	
}


void EXTI0_1_IRQHandler(void)
{ 		    		    				     		    
	EXTI_ClearITPendingBit(EXTI_Line0); // ���LINE10�ϵ��жϱ�־λ	
}


//PA0 WKUP���ѳ�ʼ��
void WKUP_Pin_Init(void)
{	
  GPIO_InitTypeDef  GPIO_InitStructure;  		  
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);//ʹ��GPIOA�͸��ù���ʱ��

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;	 //PA.0
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��IO
	
    //ʹ���ⲿ�жϷ�ʽ
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource0);	//�ж���0����GPIOA.0

  EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//���ð������е��ⲿ��·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//�����ⲿ�ж�ģʽ:EXTI��·Ϊ�ж�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //�����ش���
 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	// ��ʼ���ⲿ�ж�

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure); 
	
}

