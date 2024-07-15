#include "TIM.h"
#include "dht11.h"
//�߼���ʱ����TIM1��TIM8����ͨ�ö�ʱ����TIM2��TIM3��TIM4��TIM5����������ʱ����TIM6��TIM7��
extern u16 timeCount1;
extern u16 timeCount2;
extern u16 timeCount3;
u8 temp1=0;
u8 temp2=0;
u8 temp3=0;
void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_InternalClockConfig(TIM2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//TIME_OUT = (TIM_Period+1)*(TIM_Prescaler+1)/TIM_CLK
	//����ʱ��0.5sһ���жϣ�����������ת��ʶ��
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM����ģʽ
	TIM_TimeBaseStructure.TIM_Period = 5000-1;//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//��λֵ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//�����ʱ����־λ
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//��ʱ�������ж�Դ
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	//������ʱ��
	TIM_Cmd(TIM2, ENABLE);
}

//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIM2�����жϱ�־ 
			temp1++;
			temp2++;
			temp3++;
			if(temp1 == 15)//7.5��
			{
				timeCount1=1;
				temp1 = 0;
			}
			if(temp2 == 50)//25��
			{
				timeCount2=1;
				temp2 = 0;
			}
			if(temp3 == 25)//12.5��
			{
				timeCount3=1;
				temp3 = 0;
			}
		}
}
