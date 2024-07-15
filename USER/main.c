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

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n" //连接Onenet的MQTT服务器地址+端口
extern u8 DHT11_Value[4];
u16 ADC1_Value[2]={0,0};//ADC1采集参考电压和内部温度的ADC数值
u16 timeCount1 = 0;	//发送标识符1
u16 timeCount2 = 0;	//发送标识符2
u16 timeCount3 = 0;	//发送标识符3
int main(void)
{	
	u8 *dataPtr = NULL;

	//硬件初始化
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
	MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)&ADC1_Value,2);//DMA初始化
	delay_ms(50);
	ADC1_Init();//ADC初始化+ADC传输(DMA初始化放在ADC初始化之前，ADC传输放在DMA传输之前(防止数据错位))
	delay_ms(50);
	MYDMA_Enable(DMA1_Channel1);//DMA传输
	delay_ms(50);
	//ESP8266初始化
	ESP8266_Init();
	delay_ms(50);
	//连接Onenet
	printf("正在连接Onenet服务器......\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);
	printf("连接Onenet服务器成功！\r\n");
	//接入Onenet平台
	while(OneNet_DevLink())
		delay_ms(500);
	//订阅Onenet主题
	OneNET_Subscribe();
	
	
  while(1)
	{			
		/*获取DHT11数据*/
		if(timeCount3 == 1)//TIM2定时器到了就置一等待进入数据读取
		{
			DHT11_GetData();//获取DHT11数据，等待后续封包发送
			timeCount3 = 0;//复位标识符3
		}
		/*获取DHT11数据*/

		/*发送数据包*/
		if(timeCount1 == 1)
		{
			printf("\nDHT11湿度：%d.%d DHT11温度：%d.%d\r\n",DHT11_Value[0],DHT11_Value[1],DHT11_Value[2],DHT11_Value[3]);
			printf("发送数据到Onenet中......\r\n");
			OneNet_SendData();//发送数据到Onenet
			
			timeCount1 = 0;//复位间隔变量1
			ESP8266_Clear();//清空esp8266缓存
		}
		/*发送数据包*/
		
		/*发送心跳包*/
		if(timeCount2 == 1)
		{
			MQTT_Ping();//发送心跳包查看是否与服务器断开，若断开自动重连
			timeCount2 = 0;//复位间隔变量2
			ESP8266_Clear();//清空esp8266缓存
		}
		/*发送心跳包*/
		
		/*接受数据*/
		dataPtr = ESP8266_GetIPD(500);//获取平台返回esp8266的数据
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);//解析数据(平台操作单片机功能)
			ESP8266_Clear();
		/*接受数据*/
	}
}

