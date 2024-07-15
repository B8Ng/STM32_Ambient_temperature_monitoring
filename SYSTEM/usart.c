//引用头文件
#include "usart.h"
#include "delay.h"
/*
函数名：USART1_Init(u32 baud)
参数：波特率
作用：串口一初始化
*/
void USART1_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA和USART1的外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=baud;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收到数据中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
	
	printf("USART1已连接成功！\n");
}
/*
函数名：USART2_IRQHandler(void)
参数：无
作用：串口二中断函数
*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{USART_ClearFlag(USART2, USART_FLAG_TC);}

	USART_SendData(USART2,USART_ReceiveData(USART2));
}

/*
函数名：USART2_Init(u32 baud)
参数：波特率
作用：串口二初始化
*/
void USART2_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启GPIOA和USART2的外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//TX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=baud;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//接收到数据中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStructure);
	
	printf("USART2已连接成功！\n");
}

/*
函数名：fputc(int ch, FILE *f)
参数：无
作用：printf函数重定向到USART1
*/
int fputc(int ch, FILE *f)
{

	USART_ClearFlag(USART2,USART_FLAG_TC);//清除TC初始值
 USART_SendData(USART2, (uint8_t) ch);
 

 while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
 
 return ch;
}
/*
函数名：Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
参数：USARTx，字符串，长度
作用：串口字符串发送
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}

}
