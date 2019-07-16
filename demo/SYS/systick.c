#include "systick.h"
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

void STick_Handler(void)
{
	#ifdef NO_ALARM
	#else
	//�������й����н��б���״̬���
	if (__READ_ALARM_FLAG == 0)
	{
		if (READ_ALARM)
		{
			__WRITE_ALARM_FLAG(1);
			//
		}
	}
	#endif
	
	sig_led_routing(10);
}
	