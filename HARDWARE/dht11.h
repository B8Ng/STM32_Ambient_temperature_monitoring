#ifndef __DHT11_H__
#define __DHT11_H__
#include "stm32f10x.h"
#define DATA_IN GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define DATA_OUT GPIO_ResetBits(GPIOB,GPIO_Pin_12) 
#define Read_Data GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)

void DHT11_OUT(void);//����Ϊ���ģʽ
void DHT11_IN(void);//����Ϊ����ģʽ
void DHT11_Rst(void);//ģ�鸴λ
u8 DHT11_GetByte(void);//��ȡһ���ֽ�����
void DHT11_GetData(void);//��ȡ����
	
#endif
