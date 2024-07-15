#include "ADC.h"
#include "delay.h"
#include "stdio.h"
#include "DMA.h"
extern u16 ADC1_Value[2];//ADC1数据缓存数组
void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;		//定义ADC结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//使能ADC传输
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC分频(不超过14MHz)
	
	ADC_DeInit(ADC1);//复位重置ADC设置
	// ADC1通道配置
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式(通道1->通道2->通道3->……)
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不开启外部转化
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右
	ADC_InitStructure.ADC_NbrOfChannel = 2;//转换通道个数
	ADC_Init(ADC1,&ADC_InitStructure);	//初始化ADC
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);	//设置ADC通道，顺序，采样时长
	ADC_RegularChannelConfig(ADC1,ADC_Channel_17,2,ADC_SampleTime_239Cycles5);	//设置ADC通道，顺序，采样时长
	ADC_TempSensorVrefintCmd(ENABLE);//温度传感器使能
	ADC_DMACmd(ADC1,ENABLE);//ADC的DMA传输使能
	ADC_Cmd(ADC1,ENABLE);//使能ADC1开始转换
	ADC_ResetCalibration(ADC1);//复位ADC1
	while(ADC_GetResetCalibrationStatus(ADC1));//等待复位完毕
	ADC_StartCalibration(ADC1);//校准ADC1
	while(ADC_GetCalibrationStatus(ADC1));//等待校准完毕
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件触发ADC转换
	printf("ADC1初始化成功！\n"); 
}

double Get_Value(void)
{
	u8 i;
	u16 cout=0;
	u32 temp;
	double temperate;
	double Voltage;
	for(i=0;i<20;i++)
		{
			cout += ADC1_Value[0];
			delay_us(60);
		}
		
		
	temp = cout/20;//取平均值
	printf("\nADC采集数值：%.2f\n",(float)temp);
		
	Voltage = 1.2*((float)temp/1505.0);
	printf("校准电压：%.2f\n",(float)Voltage);
//		
//	temperate=(float)temp*(3.3/4096); //电压值
//	printf("直接测电压：%.2f\n",(float)temperate);
		
	temperate=(1.43-Voltage)/0.0043+25; //转换为温度值
	printf("芯片温度：%.2f\n",(float)temperate);
		
		
	return temperate;//返回真实的温度
}
