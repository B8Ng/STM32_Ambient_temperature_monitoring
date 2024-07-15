#ifndef __DMA_H__
#define __DMA_H__
#include "stm32f10x.h"

void MYDMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);

#endif
