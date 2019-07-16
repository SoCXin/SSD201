#include "usart3.h"
#include "misc.h"
#include "combuf.h"
#include "../util/define.h"

COM_BUF_S g_comBuff3 = {0};

void USART3_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* 配置串口1 （USART1） 时钟*/
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOG, ENABLE);//使能GPIOA,G时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA,G时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART2时钟
    /*串口GPIO端口配置*/
    /* 配置串口1 （USART1 Tx (PA.09)）*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    /* 配置串口1 USART1 Rx (PA.10)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 串口1模式（USART1 mode）配置 */
    USART_InitStructure.USART_BaudRate = 115200;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    USART_ClearFlag(USART3,USART_FLAG_TC);
    
    USART_Cmd(USART3, ENABLE); //使能串口 
}

/*Usart1 NVIC 配置*/
void NVIC3_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    /* 使能串口1中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    /*IRQ通道使能*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1*/
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputcStr3(int ch)
{
    /* 将Printf内容发往串口 */
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
