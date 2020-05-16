#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "ListTimer.h"
#include "LedApp.h"
#include "gpio.h"






UTIL_TIME *LedAppUt;












void LedAppFlickCallBack(void *data)
{
	static int flicker = 0;
	
	if(flicker==LED_ON)
	{
		gpio_output(LED_D3, LED_ON);
	}
	else
	{
		gpio_output(LED_D3, LED_OFF);
	}
	flicker = !flicker;
}

void LedAppFlicker(void)
{
	if(LedAppUt) del_timer(&LedAppUt);
	add_timer(1, 1, LedAppFlickCallBack, NULL, &LedAppUt);
}

void LedAppLight(void)
{
	if(LedAppUt) del_timer(&LedAppUt);
	gpio_output(LED_D3, LED_ON);
}

void LedAppOff(void)
{
	if(LedAppUt) del_timer(&LedAppUt);
	gpio_output(LED_D3, LED_OFF);
}


void LedAppInit(void)
{
	LedAppUt = NULL;
	if(0 != gpio_init(LED_D3,GPIO_OUT_PUT)) 
	{
		printf("led gpio init failled\n");
		return;
	}
	gpio_output(LED_D3, LED_ON);
//	LedAppFlicker();
}








