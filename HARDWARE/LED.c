#include "stm32f10x.h"

#include "LED.h"

LED_INFO led_info = {0};

void Led_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;				
	gpio_initstruct.GPIO_Pin = GPIO_Pin_13;						
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				
	
	GPIO_Init(GPIOC, &gpio_initstruct);						
	
	Led_Set(LED_OFF);											

}


void Led_Set(_Bool status)
{
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, status == LED_ON ? Bit_RESET : Bit_SET);		//���status����LED_ON���򷵻�Bit_SET�����򷵻�Bit_RESET
	
	led_info.Led_Status = status;

}
