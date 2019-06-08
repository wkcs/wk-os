#ifndef __LED_H
#define __LED_H	 
#include "stm32f10x.h"
#include "gpio.h"

#define LED0 PBout(5)// PB5
#define LED1 PEout(5)// PE5	

void LED_Init(void);//初始化

		 				    
#endif
