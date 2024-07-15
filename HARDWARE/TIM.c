#include "TIM.h"
#include "dht11.h"
//高级定时器（TIM1、TIM8）；通用定时器（TIM2、TIM3、TIM4、TIM5）；基本定时器（TIM6、TIM7）
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
	//本计时器0.5s一次中断，用于任务跳转标识符
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM计数模式
	TIM_TimeBaseStructure.TIM_Period = 5000-1;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//复位值
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//清除定时器标志位
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//定时器更新中断源
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	//启动定时器
	TIM_Cmd(TIM2, ENABLE);
}

//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIM2更新中断标志 
			temp1++;
			temp2++;
			temp3++;
			if(temp1 == 15)//7.5秒
			{
				timeCount1=1;
				temp1 = 0;
			}
			if(temp2 == 50)//25秒
			{
				timeCount2=1;
				temp2 = 0;
			}
			if(temp3 == 25)//12.5秒
			{
				timeCount3=1;
				temp3 = 0;
			}
		}
}
