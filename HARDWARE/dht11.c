#include "dht11.h"
#include "delay.h"
#include "usart.h"
#include "esp8266.h"
/***************STM32F103C8T6**********************
 * 文件名  ：DHT11.c
 * 描述    ：DHT11传感器
 * 备注    : DHT11温度湿度传感器
 * 接口    ：PB12-DATA
***************************************************/
u8 DHT11_Value[4]={0,0,0,0};//温湿度数据存储数组
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
	
	for(i=0;i<8;i++) //1个数据就是1个字节byte，1个字节byte有8位bit
	{
		while( Read_Data == 0); //从1bit开始，低电平变高电平，等待低电平结束
		delay_us(30); //延迟30us是为了区别数据0和数据1，0只有26~28us
		
		data <<= 1; //左移
		
		if( Read_Data == 1 ) //如果过了30us还是高电平的话就是数据1
		{
			data |= 1; //数据+1
		}
		
		while( Read_Data == 1 ); //高电平变低电平，等待高电平结束
	}
	
	return data;
}

//采样周期2s以上(避免温度不准)
void DHT11_GetData(void)
{
	unsigned int R_H,R_L,T_H,T_L;
	unsigned char RH,RL,TH,TL,CHECK;
	
	DHT11_Rst(); //主机发送信号
	DATA_IN; //拉高电平
	
	if( Read_Data == 0 ) //判断DHT11是否响应
	{
		while( Read_Data == 0); //低电平变高电平，等待低电平结束
		while( Read_Data == 1); //高电平变低电平，等待高电平结束
		
		R_H = DHT11_GetByte();
		R_L = DHT11_GetByte();
		T_H = DHT11_GetByte();
		T_L = DHT11_GetByte();
		CHECK = DHT11_GetByte(); //接收5个数据
		
		DATA_OUT; //当最后一bit数据传送完毕后，DHT11拉低总线 50us
		delay_us(55); //这里延时55us
		DATA_IN; //随后总线由上拉电阻拉高进入空闲状态。
		
		if(R_H + R_L + T_H + T_L == CHECK) //和检验位对比，判断校验接收到的数据是否正确
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
