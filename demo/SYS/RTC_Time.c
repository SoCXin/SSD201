/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               RTC_Time.c
** Descriptions:            The RTC application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/*******************************************************************************
* ���ļ�ʵ�ֻ���RTC�����ڹ��ܣ��ṩ�����յĶ�д��������ANSI-C��time.h��
* 

* RTC�б����ʱ���ʽ����UNIXʱ�����ʽ�ġ���һ��32bit��time_t������ʵΪu32��
*
* ANSI-C�ı�׼���У��ṩ�����ֱ�ʾʱ�������  �ͣ�
* time_t:   	UNIXʱ�������1970-1-1��ĳʱ�侭����������
* 	typedef unsigned int time_t;
* 
* struct tm:	Calendar��ʽ����������ʽ��
*   tm�ṹ���£�
*   struct tm {
*   	int tm_sec;   // �� seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // �� minutes after the hour, 0 to 59
*		int tm_hour;  // ʱ hours since midnight, 0 to 23
*		int tm_mday;  // �� day of the month, 1 to 31
*		int tm_mon;   // �� months since January, 0 to 11
*		int tm_year;  // �� years since 1900
*		int tm_wday;  // ���� days since Sunday, 0 to 6
*		int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
* 		int tm_isdst; // ����ʱ����Daylight Savings Time flag
* 		...
* 	}
* 	����wday��yday�����Զ����������ֱ�Ӷ�ȡ
* 	mon��ȡֵΪ0-11
*	***ע��***��
*	tm_year:��time.h���ж���Ϊ1900�������ݣ���2008��Ӧ��ʾΪ2008-1900=108
* 	���ֱ�ʾ�������û���˵����ʮ���Ѻã�����ʵ�нϴ���졣
* 	�����ڱ��ļ��У����������ֲ��졣
* 	���ⲿ���ñ��ļ��ĺ���ʱ��tm�ṹ�����͵����ڣ�tm_year��Ϊ2008
* 	ע�⣺��Ҫ����ϵͳ��time.c�еĺ�������Ҫ���н�tm_year-=1900
* 
* ��Ա����˵����
* struct tm Time_ConvUnixToCalendar(time_t t);
* 	����һ��Unixʱ�����time_t��������Calendar��ʽ����
* time_t Time_ConvCalendarToUnix(struct tm t);
* 	����һ��Calendar��ʽ���ڣ�����Unixʱ�����time_t��
* time_t Time_GetUnixTime(void);
* 	��RTCȡ��ǰʱ���Unixʱ���ֵ
* struct tm Time_GetCalendarTime(void);
* 	��RTCȡ��ǰʱ�������ʱ��
* void Time_SetUnixTime(time_t);
* 	����UNIXʱ�����ʽʱ�䣬����Ϊ��ǰRTCʱ��
* void Time_SetCalendarTime(struct tm t);
* 	����Calendar��ʽʱ�䣬����Ϊ��ǰRTCʱ��
* 
* �ⲿ����ʵ����
* ����һ��Calendar��ʽ�����ڱ�����
* struct tm now;
* now.tm_year = 2008;
* now.tm_mon = 11;		//12��
* now.tm_mday = 20;
* now.tm_hour = 20;
* now.tm_min = 12;
* now.tm_sec = 30;
* 
* ��ȡ��ǰ����ʱ�䣺
* tm_now = Time_GetCalendarTime();
* Ȼ�����ֱ�Ӷ�tm_now.tm_wday��ȡ������
* 
* ����ʱ�䣺
* Step1. tm_now.xxx = xxxxxxxxx;
* Step2. Time_SetCalendarTime(tm_now);
* 
* ��������ʱ��Ĳ�
* struct tm t1,t2;
* t1_t = Time_ConvCalendarToUnix(t1);
* t2_t = Time_ConvCalendarToUnix(t2);
* dt = t1_t - t2_t;
* dt��������ʱ��������
* dt_tm = mktime(dt);	//ע��dt�����ƥ�䣬ansi���к���Ϊ�����ݣ�ע�ⳬ��
* ����Բο�������ϣ�����ansi-c��ĸ�ʽ������ȹ��ܣ�ctime��strftime��
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "RTC_Time.h"
#include <stm32f10x_bkp.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_rcc.h>
#include "misc.h"
#include "common.h"
#include "bsp.h"

/* Private define ------------------------------------------------------------*/
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */  

/* Private function prototypes -----------------------------------------------*/
void Time_Set(u32 t);

/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar
* Description    : ת��UNIXʱ���Ϊ����ʱ��
* Input          : - t: ��ǰʱ���UNIXʱ���
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	/* localtimeת�������tm_year�����ֵ����Ҫת�ɾ���ֵ */
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix
* Description    : д��RTCʱ�ӵ�ǰʱ��
* Input          : - t: struct tm
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  /* �ⲿtm�ṹ��洢�����Ϊ2008��ʽ	*/
						/* ��time.h�ж������ݸ�ʽΪ1900�꿪ʼ����� */
						/* ���ԣ�������ת��ʱҪ���ǵ�������ء�*/
	return mktime(&t);
}


/*******************************************************************************
* Function Name  : Time_GetUnixTime
* Description    : ��RTCȡ��ǰʱ���Unixʱ���ֵ
* Input          : None
* Output         : None
* Return         : time_t
* Attention		 : None
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime
* Description    : ��RTCȡ��ǰʱ�������ʱ�䣨struct tm��
* Input          : None
* Output         : None
* Return         : struct tm
* Attention		 : None
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime
* Description    : ��������Unixʱ���д��RTC
* Input          : - t: time_t 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime
* Description    : ��������Calendar��ʽʱ��ת����UNIXʱ���д��RTC
* Input          : - t: struct tm
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  RCC_LSICmd(ENABLE);
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);  

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : USART_Scanf
* Description    : USART Receive
* Input          : - min_value: 
*                  - max_value:
*                  - lenght:
* Output         : None
* Return         : uint8_t
* Attention		 : None
*******************************************************************************/
static uint16_t USART_Scanf(uint32_t min_value,uint32_t max_value,uint8_t lenght)
{
  uint16_t index = 0;
  uint32_t tmp[4] = {0, 0, 0, 0};


  return index;
}

/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Regulate(void)
{
  struct tm time;

  memset(&time, 0 , sizeof(time) );	/* ��սṹ�� */

  printf("=======================Time Settings==========================\r\n");
  printf("Please Set Years between 1970 to 2037\r\n");

  while ( time.tm_year>2037  || time.tm_year<1970)
  {   
    time.tm_year = USART_Scanf(1970,2037,4);
  }
  printf("Set Years:  %d\r\n", time.tm_year);

  printf("Please Set Months between 01 to 12\r\n");
  while (time.tm_mon >12 || time.tm_mon < 1 )
  {
    time.tm_mon= USART_Scanf(1,12,2)-1;
  }
  printf("Set Months:  %d\r\n", time.tm_mon);

  printf("Please Set Days between 01 to 31\r\n");
  while (time.tm_mday >31 ||time.tm_mday <1 )
  {
    time.tm_mday = USART_Scanf(1,31,2);
  }
  printf("Set Days:  %d\r\n", time.tm_mday);

  printf("Please Set Hours between 01 to 23\r\n");
  while (time.tm_hour >23 ||time.tm_hour <1 )
  {
    time.tm_hour = USART_Scanf(1,23,2);
  }
  printf("Set Hours:  %d\r\n", time.tm_hour);

  printf("Please Set Minutes between 01 to 59\r\n");
  while (time.tm_min >59 || time.tm_min <1 )
  {
    time.tm_min = USART_Scanf(1,59,2);
  }
  printf("Set Minutes:  %d\r\n", time.tm_min);

  printf("Please Set Seconds between 01 to 59\r\n");
  while (time.tm_sec >59 || time.tm_sec <1 )
  {
    time.tm_sec = USART_Scanf(1,59,2);
  }
  printf("Set Seconds:  %d\r\n", time.tm_sec);
  /* Return the value to store in RTC counter register */
  Time_SetCalendarTime(time);  
}

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC Initialization
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None	
*******************************************************************************/



static void RTC_NVIC_Config(void)
{	
  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);		//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}


int RTC_Init(void)
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  
	RCC_LSICmd(ENABLE);
	if (GET_RTCINIT_FLAG != 1)		//��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
	{
		printf("RTC  ���³�ʼ�� %d  \r\n",GET_RTCINIT_FLAG);
		BKP_DeInit();	//��λ�������� 	
		//RCC_LSEConfig(RCC_LSE_OFF);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET&&temp<250)	//���ָ����RCC��־λ�������,�ȴ����پ������
		{
			temp++;
			//rt_thread_sleep(1);
		}
		if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
		//RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������	
		RTC_SetPrescaler(40000); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		//RTC_Set(2015,1,14,17,42,55);  //����ʱ��	
		
		RTC_ITConfig(RTC_IT_ALR, ENABLE);	//ʹ��RTC���ж�
		RTC_NVIC_Config();//RCT�жϷ�������	
		
		RTC_ExitConfigMode(); //�˳�����ģ�
		SET_RTCINIT_FLAG;  
		//BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
	}
	else//ϵͳ������ʱ
	{

		RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}
		    				     	
	return 0; //ok

}		 				    
//RTCʱ���ж�
//ÿ�봥��һ��  
//extern u16 tcnt; 
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//�����ж�
	{
	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
	{
		printf("alarm .\r\n");
	} 				  								 

	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW|RTC_IT_ALR);		//�������ж�
		  	    						 	   	 
}

////
//void RTC_Init(void)
//{

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR , ENABLE);
//	PWR_BackupAccessCmd(ENABLE);//???????,??????????


//	if(BKP_ReadBackupRegister(BKP_DR1) != 0xAA00)//Vbat?VDD??
//	{
//		BKP_DeInit();  //??????,?BKP?????
//		RCC_LSICmd(ENABLE);//????????LSI
//		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == 0) 
//		{
//		}


//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
//		//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//????????LSE?RTC???

//		RCC_RTCCLKCmd(ENABLE);//????RTC??


//		RTC_WaitForLastTask();//??????RTC????(??,?????RTC?????,????????????)
//		RTC_WaitForSynchro();//???APB1???,???RTC???


//		///////////////// ////(???????)////////////////////////////

//		RTC_EnterConfigMode();//??RTC??
//		RTC_WaitForLastTask();//??????RTC????


//		RTC_SetPrescaler(40000);//????1HZ
//		RTC_WaitForLastTask();//??????RTC????

//		RTC_ITConfig(RTC_IT_ALR , DISABLE);//??????.  ???RTC_IT_SEC

//		RTC_WaitForLastTask();//??????RTC????



//		RTC_WaitForLastTask();//??????RTC????

//		RTC_ExitConfigMode(); //??RTC????


//		BKP_WriteBackupRegister(BKP_DR1, 0xAA00);//???????????

//	}

//	else//????????????
//	{

//		RTC_WaitForSynchro();//???APB1???,???RTC???

//		RTC_EnterConfigMode();//??RTC??

//		RTC_ITConfig(RTC_IT_ALR, DISABLE);

//		RTC_WaitForLastTask();//??????RTC????

//		RTC_ExitConfigMode(); //??RTC????
//		RTC_WaitForLastTask();//??????RTC????

//	}
//  return;
//}

/*******************************************************************************
* Function Name  : Time_Display
* Description    : Printf Time
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void Time_Display(void)
{
   struct tm time;
   time = Time_GetCalendarTime();
   printf("Time: %d-%d-%d   %02d:%02d:%02d \r\n", time.tm_year, \
                   time.tm_mon+1, time.tm_mday,\
                   time.tm_hour, time.tm_min, time.tm_sec);
}


