#ifndef __USART_H__
#define __USART_H__
#include "stm32f10x.h"
#include "stdio.h"

void USART1_Init(u32 baud);
void USART2_IRQHandler(void);
void USART2_Init(u32 baud);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
int fputc(int ch, FILE *f);

#endif
