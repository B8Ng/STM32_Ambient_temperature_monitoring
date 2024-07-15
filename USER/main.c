#include "stm32f10x.h"
#include "esp8266.h"
#include "delay.h"
#include "usart.h"
#include "ADC.h"
#include "LED.h"
#include "DMA.h"
#include "dht11.h"
#include "onenet.h"
#include "mqttKit.h"
#include "TIM.h"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n" //����Onenet��MQTT��������ַ+�˿�
extern u8 DHT11_Value[4];
u16 ADC1_Value[2]={0,0};//ADC1�ɼ��ο���ѹ���ڲ��¶ȵ�ADC��ֵ
u16 timeCount1 = 0;	//���ͱ�ʶ��1
u16 timeCount2 = 0;	//���ͱ�ʶ��2
u16 timeCount3 = 0;	//���ͱ�ʶ��3
int main(void)
{	
	u8 *dataPtr = NULL;

	//Ӳ����ʼ��
  delay_init(72);
	delay_ms(50);
	USART2_Init(115200);
	delay_ms(50);
	USART1_Init(115200);
	delay_ms(50);
	Led_Init();
	delay_ms(50);
	TIM2_Init();
	delay_ms(50);
	MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&ADC1_Value,2);//DMA��ʼ��
	delay_ms(50);
	ADC1_Init();//ADC��ʼ��+ADC����(DMA��ʼ������ADC��ʼ��֮ǰ��ADC�������DMA����֮ǰ(��ֹ���ݴ�λ))
	delay_ms(50);
	MYDMA_Enable(DMA1_Channel1);//DMA����
	delay_ms(50);
	//ESP8266��ʼ��
	ESP8266_Init();
	delay_ms(50);
	//����Onenet
	printf("��������Onenet������......\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	printf("����Onenet�������ɹ���\r\n");
	//����Onenetƽ̨
	while(OneNet_DevLink())
		delay_ms(500);
	//����Onenet����
	OneNET_Subscribe();
	
	
  while(1)
	{			
		/*��ȡDHT11����*/
		if(timeCount3 == 1)//TIM2��ʱ�����˾���һ�ȴ��������ݶ�ȡ
		{
			DHT11_GetData();//��ȡDHT11���ݣ��ȴ������������
			timeCount3 = 0;//��λ��ʶ��3
		}
		/*��ȡDHT11����*/

		/*�������ݰ�*/
		if(timeCount1 == 1)
		{
			printf("\nDHT11ʪ�ȣ�%d.%d DHT11�¶ȣ�%d.%d\r\n",DHT11_Value[0],DHT11_Value[1],DHT11_Value[2],DHT11_Value[3]);
			printf("�������ݵ�Onenet��......\r\n");
			OneNet_SendData();//�������ݵ�Onenet
			
			timeCount1 = 0;//��λ�������1
			ESP8266_Clear();//���esp8266����
		}
		/*�������ݰ�*/
		
		/*����������*/
		if(timeCount2 == 1)
		{
			MQTT_Ping();//�����������鿴�Ƿ���������Ͽ������Ͽ��Զ�����
			timeCount2 = 0;//��λ�������2
			ESP8266_Clear();//���esp8266����
		}
		/*����������*/
		
		/*��������*/
		dataPtr = ESP8266_GetIPD(500);//��ȡƽ̨����esp8266������
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);//��������(ƽ̨������Ƭ������)
			ESP8266_Clear();
		/*��������*/
	}
}

