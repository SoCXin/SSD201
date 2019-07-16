/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm320518_eval.h"
#include "common.h"
#include "udp_proto.h"
#include "rtc.h"
#include "flash.h"
#include "config.h"
#include "dcmi_ov2640.h"
#include <stdlib.h>
#include "ota.h"
#include "alarm.h"
#include "mem.h"
#include "setting.h"
#include "MDK-ARM/vtime.h"
#include "signalled.h"
#include "systick.h"
#include "ir.h"
#include "testpoint.h"


static void running(void);
static void test_timer(void *p);
static void __lowpower_test(void);
static void __lowpower(void);

//Heap_Size
#ifdef MAKE_LIB
int entry(void)
#else
int main(void)
#endif
{
	
	//���ݳ�ʼ��
	init_mem();
	init_utimer();
	init_task();
	mainloop_init();
	
	//��һ�׶ε�Ӳ����ʼ��
	SysTick_Config(SystemCoreClock / 100);
	
	debugport_init();
	RTC_Config();
	WKUP_Pin_Init();
	enable_iwdg();
	init_testpoint_pin();
	ov2640_poweroff(); //��֤�ϵ���������״̬����Ӧ��������ʱ�ĸ���״̬
	
	#ifdef USE_OTA
	//�����OTA��ô����Ҫ�����ж�������
	//VectorTable_Set();  ��ʱû��OTA�����Ȳ�����
	#endif

	printf("\r\n\r\n\r\n\r\n\r\n");
	printf("%s%s%s\r\n",WARN_STR2______,__DATE__,__TIME__);
	printf("%s%s \r\n",WARN_STR_______,VERSION_TIM_STR);
	printf("SystemCoreClock %d \r\n",SystemCoreClock);
	
	//�͹��Ĵ��������Ҫ����͹��ģ������
	__lowpower();
	
	
	#ifdef NO_ALARM
	#else
	//�ж�����б�������ֱ�ӽ���������
	if (__READ_ALARM_FLAG == 1)
	{
		goto RUNNING;
	}
	#endif
	
	
	//��ȡ����״̬��������ش�������������
	DEBUG_VALUE(__READ_STANDBY_STATUS);
	switch(__READ_STANDBY_STATUS)
	{
		case STANDBY_STATUS_ALARM:
			break;
		case STANDBY_STATUS_INIT:
			read_photo_ov2640(0);
			mdata.need_uload_cnt = 1; //������Ҫ�ϴ���ͼƬ����,��Ϊ���״ο����������ϴ�1�žͿ���
			__WRITE_COUNTER(0);
			mdata.pictype = 1;
			printf("First START !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
			break;
		case STANDBY_STATUS_PAIZHAO_1:
			
		
			DEBUG_VALUE(__READ_COUNTER);
			if (__READ_COUNTER < PAIZHAO_COUNT)
			{
				read_photo_ov2640(__READ_COUNTER);
				__ADD_COUNTER; //�������� +1
				
			}
			
			//���պ󣬼���Ƿ��Ѿ�������������������ˣ�������״̬�����������´��ϴ�����
			if (PAIZHAO_COUNT >= PAIZHAO_COUNT){
				__WRITE_STANDBY_STATUS(STANDBY_STATUS_PAIZHAO_FINISH);
				//������һ������ʱ�䣬����������
				//��λϵͳ��������
				//__SET_NEXT_WAKEUP_TIM(60);
				
				//__SET_NEXT_WAKEUP_TIM_TIM2;
				//�����ϴ�ʱ��
				set_next_wakeup_tim(__GET_UPLOAD_TIM);
				sys_shutdown();
				
				
			}
			
			break;
		case STANDBY_STATUS_PAIZHAO_FINISH:
			//������ɺ�������Ҫ�ϴ������մ������������ϴ�״̬
			mdata.need_uload_cnt = PAIZHAO_COUNT;
			break;
		default:
			break;
	}
	
	DEBUG_VALUE(__READ_STANDBY_STATUS);
	
	/*********************************************
	RUNNNING ����
	����ǰ���е�״̬���Ͳ������ú�֮
	����ܽ������������
	**********************************************/
	RUNNING:
	
	//�����ڴ�
	mem = (struct MEM_DATA *)alloc_mem(__FILE__,__LINE__,sizeof(struct MEM_DATA));
	memset(mem,0x0,sizeof(struct MEM_DATA));
	RC;
	
	init_sig_led_gpio();
	adc_init();
	mdata.voltage = get_bat_vol();
	printf("VOLTAGE  %d\r\n",mdata.voltage);
	USART2_Configuration();
	
	


	//����һ����ʱ��������ι������
	timerlist[0].func = test_timer;
	timerlist[0].arg = 0;
	timerlist[0].type = 0;
	timerlist[0].__dangqian = 0;
	timerlist[0].__zhouqi = 10;
	timerlist[0].enable = 1;

	
	running();
	
	return 0;
}

static void running(void)
{
	for(;;)
  {
		run_task();
		mainloop();
  }
}


static void test_timer(void *p)
{
	feed_iwdg();
	printf("current system runnning %d ms\r\n",__time_100ms_cnt[TIMER_100MS_SYSTEMTIME]*100);
	trace_mem();
	//��˸LED
}

static void __lowpower_test(void)
{
	
	printf("run %d %d ...................\r\n", RTC_GetCounter(),RTC_GetCounter()-__READ_NEXT_WAKEUP_TIM);
	utimer_sleep(5000);
	printf("Sys_Standby %d ......................\r\n",READ_ALARM);
	printf("Sys_Standby %d %d ...................\r\n", RTC_GetCounter(),RTC_GetCounter()-__READ_NEXT_WAKEUP_TIM);
	Sys_Enter_Standby();
}

static void __lowpower(void)
{
	int i=0;
	
	__WRITE_RESET_COUNT;
	
	printf("__RESET_COUNT %d \r\n",__READ_RESET_COUNT);
	
	printf("TRACE_CONF:\r\n");
	for(i=0;i<(CONFIG_DATA_SIZE-CONFNIG_RAW_DATA_SIZE);i++)
	{
		printf("%c",((char*)(CONFIG_DATA_ADDR+CONFNIG_RAW_DATA_SIZE))[i]);
		if (((char*)(CONFIG_DATA_ADDR+CONFNIG_RAW_DATA_SIZE))[i] == '}')
			break;
	}
	printf("\r\n");
	
	
	//printf("TRACE JSON: %s\r\n",(char*)(CONFIG_DATA_ADDR+CONFNIG_RAW_DATA_SIZE+4));
	
	//wait register ???
	utimer_sleep(20);	
	
	#ifdef NO_ALARM
	#else
	
	DEBUG_VALUE(__READ_ALARM_FLAG);
	switch(__READ_ALARM_FLAG)
	{
		case 0:
			if (READ_ALARM == 1)
			{
				__WRITE_ALARM_FLAG(1);
				goto ret;
			}
			break;
		case 1:
			goto ret;
		case 2:
			if (READ_ALARM == 0)
			{
				__WRITE_ALARM_FLAG(0);
			}
			break;
		default:
			break;
	}
	#endif
	
	printf("nex wakeup status %d time : %d  current tim : %d -- %d \r\n",__READ_STANDBY_STATUS,__READ_NEXT_WAKEUP_TIM,RTC_GetCounter(),RTC_GetCounter() -__READ_NEXT_WAKEUP_TIM);
	
	// rtc �����ж�
	if (__READ_NEXT_WAKEUP_TIM < RTC_GetCounter())
	{
		//mdata.status = 
		//�����ʱ�ˣ�˵��Ӧ���ϴ�ͼƬ�� 
	}else{
		//��������
		
		//������γ�����Ϊ����߻��Ѿ��ȣ���RTC��ʱ��Ū���Ժ󣬾Ϳ���ȥ����δ���
		unsigned int __shengyufshijian = __READ_NEXT_WAKEUP_TIM - RTC_GetCounter();
		
		//printf("run %d %d ...................\r\n", RTC_GetCounter(),RTC_GetCounter()-__READ_NEXT_WAKEUP_TIM);
		
		if (__shengyufshijian < 30)
		{
			printf("Sys_Enter_Sleep...................\r\n");
			if (__shengyufshijian < 20)
			{
				if (__shengyufshijian < 10)
				{
					utimer_sleep(__shengyufshijian*1000);
					sys_shutdown();
				}
				
				utimer_sleep(10*1000);
				sys_shutdown();
			}
			
			utimer_sleep(10*1000);
			sys_shutdown();
			
		}
		
//		#ifdef FLASH_TRACE
//		
//		printf("TRACE JSON: %s\r\n",(char*)CONFIG_DATA_ADDR+CONFNIG_RAW_DATA_SIZE);
//		
//		if(mem <= 0)
//		{
//			mem = (struct MEM_DATA *)alloc_mem(__FILE__,__LINE__,sizeof(struct MEM_DATA));
//			memset(mem,0x0,sizeof(struct MEM_DATA));
//			RC;

//		}
//		printf("FLASH TRACE:[%d][%s]\r\n",mem->cfg_data.trace_time,mem->cfg_data.trace);
//		DEBUG_VALUE(__GET_LAST_PAIZHAOTIM);
//		DEBUG_VALUE(__GET_UPLOAD_TIM);
//		DEBUG_VALUE(mem->cfg_data.last_voltage);
//		
//		#endif
		
		printf("Sys_Enter_Standby...................\r\n");
		Sys_Enter_Standby();
		
	}
	
	ret:
	return ;
	
}



#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
