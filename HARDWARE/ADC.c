#include "ADC.h"
#include "delay.h"
#include "stdio.h"
#include "DMA.h"
extern u16 ADC1_Value[2];//ADC1���ݻ�������
void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;		//����ADC�ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ʹ��ADC����
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC��Ƶ(������14MHz)
	
	ADC_DeInit(ADC1);//��λ����ADC����
	// ADC1ͨ������
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ(ͨ��1->ͨ��2->ͨ��3->����)
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������ⲿת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//��
	ADC_InitStructure.ADC_NbrOfChannel = 2;//ת��ͨ������
	ADC_Init(ADC1,&ADC_InitStructure);	//��ʼ��ADC
	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);	//����ADCͨ����˳�򣬲���ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_17,2,ADC_SampleTime_239Cycles5);	//����ADCͨ����˳�򣬲���ʱ��
	ADC_TempSensorVrefintCmd(ENABLE);//�¶ȴ�����ʹ��
	ADC_DMACmd(ADC1,ENABLE);//ADC��DMA����ʹ��
	ADC_Cmd(ADC1,ENABLE);//ʹ��ADC1��ʼת��
	ADC_ResetCalibration(ADC1);//��λADC1
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λ���
	ADC_StartCalibration(ADC1);//У׼ADC1
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼���
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//�������ADCת��
	printf("ADC1��ʼ���ɹ���\n"); 
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
		
		
	temp = cout/20;//ȡƽ��ֵ
	printf("\nADC�ɼ���ֵ��%.2f\n",(float)temp);
		
	Voltage = 1.2*((float)temp/1505.0);
	printf("У׼��ѹ��%.2f\n",(float)Voltage);
//		
//	temperate=(float)temp*(3.3/4096); //��ѹֵ
//	printf("ֱ�Ӳ��ѹ��%.2f\n",(float)temperate);
		
	temperate=(1.43-Voltage)/0.0043+25; //ת��Ϊ�¶�ֵ
	printf("оƬ�¶ȣ�%.2f\n",(float)temperate);
		
		
	return temperate;//������ʵ���¶�
}
