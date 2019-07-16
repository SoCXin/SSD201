#include "usart3.h"
#include "misc.h"
#include "combuf.h"
#include "../util/define.h"

COM_BUF_S g_comBuff3 = {0};

void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* ���ô���1 ��USART1�� ʱ��*/
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOG, ENABLE);//ʹ��GPIOA,Gʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIOA,Gʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART2ʱ��
    /*����GPIO�˿�����*/
    /* ���ô���1 ��USART1 Tx (PA.09)��*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    /* ���ô���1 USART1 Rx (PA.10)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* ����1ģʽ��USART1 mode������ */
    USART_InitStructure.USART_BaudRate = 115200;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
    USART_ClearFlag(USART3,USART_FLAG_TC);
    
    USART_Cmd(USART3, ENABLE); //ʹ�ܴ��� 
}

/*Usart1 NVIC ����*/
void NVIC3_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    /* ʹ�ܴ���1�ж� */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    /*IRQͨ��ʹ��*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /*����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1*/
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputcStr3(int ch)
{
    /* ��Printf���ݷ������� */
    USART_SendData(USART3, (unsigned char) ch);
    while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);    
    return (ch);
}

void USART3_IRQHandler(void)
{
    unsigned char code;

    if ( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
    {
      code = USART_ReceiveData(USART3);
      g_comBuff3.buff[g_comBuff3.step % COM_BUFF_LEN] = (char)code;
      g_comBuff3.step = g_comBuff3.step + 1;
    }

    return;
}

void writeCom3(char *p)
{
    int i;
    for (i = 0; i < 1000; i = i +1 ){
        if (*(p+i) == 0) {
            fputcStr3('\0');
            break;
        }

        fputcStr3(*(p+i));
        osDelay(2);
    }
}

void cleanCom3Buf(void)
{
    memset(&g_comBuff3, 0, sizeof( COM_BUF_S));
}
