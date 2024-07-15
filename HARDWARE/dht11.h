#ifndef __DHT11_H__
#define __DHT11_H__
#include "stm32f10x.h"
#define DATA_IN GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define DATA_OUT GPIO_ResetBits(GPIOB,GPIO_Pin_12) 
#define Read_Data GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)

void DHT11_OUT(void);//设置为输出模式
void DHT11_IN(void);//设置为输入模式
void DHT11_Rst(void);//模块复位
u8 DHT11_GetByte(void);//获取一个字节数据
void DHT11_GetData(void);//获取数据
	
#endif
