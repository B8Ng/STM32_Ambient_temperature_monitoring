#include "dht11.h"
#include "delay.h"
#include "usart.h"
#include "esp8266.h"
/***************STM32F103C8T6**********************
 * �ļ���  ��DHT11.c
 * ����    ��DHT11������
 * ��ע    : DHT11�¶�ʪ�ȴ�����
 * �ӿ�    ��PB12-DATA
***************************************************/
u8 DHT11_Value[4]={0,0,0,0};//��ʪ�����ݴ洢����
void DHT11_OUT(void)
{
	GPIO_InitTypeDef GPIO_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_Structure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_Structure);
}
void DHT11_IN(void)
{
	GPIO_InitTypeDef GPIO_Structure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_Structure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_Structure);
}
void DHT11_Rst(void)
{
	DHT11_OUT();
	DATA_IN;
	delay_us(30);
	DATA_OUT;
	delay_ms(20);
	DATA_IN;
	delay_us(30);
	DHT11_IN();
}
u8 DHT11_GetByte(void)
{
	u8 i = 0;
	u8 data;
	
	for(i=0;i<8;i++) //1�����ݾ���1���ֽ�byte��1���ֽ�byte��8λbit
	{
		while( Read_Data == 0); //��1bit��ʼ���͵�ƽ��ߵ�ƽ���ȴ��͵�ƽ����
		delay_us(30); //�ӳ�30us��Ϊ����������0������1��0ֻ��26~28us
		
		data <<= 1; //����
		
		if( Read_Data == 1 ) //�������30us���Ǹߵ�ƽ�Ļ���������1
		{
			data |= 1; //����+1
		}
		
		while( Read_Data == 1 ); //�ߵ�ƽ��͵�ƽ���ȴ��ߵ�ƽ����
	}
	
	return data;
}

//��������2s����(�����¶Ȳ�׼)
void DHT11_GetData(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Rst(); //���������ź�
	DATA_IN; //���ߵ�ƽ
	
	if( Read_Data == 0 ) //�ж�DHT11�Ƿ���Ӧ
	{
		while( Read_Data == 0); //�͵�ƽ��ߵ�ƽ���ȴ��͵�ƽ����
		while( Read_Data == 1); //�ߵ�ƽ��͵�ƽ���ȴ��ߵ�ƽ����
		
		R_H = DHT11_GetByte();
		R_L = DHT11_GetByte();
		T_H = DHT11_GetByte();
		T_L = DHT11_GetByte();
		CHECK = DHT11_GetByte(); //����5������
		
		DATA_OUT; //�����һbit���ݴ�����Ϻ�DHT11�������� 50us
		delay_us(55); //������ʱ55us
		DATA_IN; //��������������������߽������״̬��
		
		if(R_H + R_L + T_H + T_L == CHECK) //�ͼ���λ�Աȣ��ж�У����յ��������Ƿ���ȷ
		{
			RH = R_H;
			RL = R_L;
			TH = T_H;
			TL = T_L;
		}
	}
	DHT11_Value[0] = RH;
	DHT11_Value[1] = RL;
	DHT11_Value[2] = TH;
	DHT11_Value[3] = TL;
}
