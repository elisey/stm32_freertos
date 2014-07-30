#ifndef UART_H
#define UART_H

#include "stm32f10x.h"

#define UART_MODULE					2
#define UART_BOUD_RATE         		115200L
#define uartSIZE_OF_RING_BUFFER		128
#define USE_FREERTOS				//uncomment this for using freeRTOS semaphore
#define UART_NO_DATA				(-1)

#if (UART_MODULE == 1)
#define USARTx						USART1
#define USART_Pin_Tx				GPIO_Pin_9
#define USART_Pin_Rx				GPIO_Pin_10
#define USART_GPIO					GPIOA
#define USARTx_IRQHandler			USART1_IRQHandler
#define USARTx_IRQn					USART1_IRQn
#define USART_RCC_INIT				do { 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);	\
											RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE); }\
									while(0)
#elif (UART_MODULE == 2)
#define USARTx						USART2
#define USART_Pin_Tx				GPIO_Pin_2
#define USART_Pin_Rx				GPIO_Pin_3
#define USART_GPIO					GPIOA
#define USARTx_IRQHandler			USART2_IRQHandler
#define USARTx_IRQn					USART2_IRQn
#define USART_RCC_INIT				do {	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);	\
											RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE); }\
									while(0)
#elif (UART_MODULE == 3)
#define USARTx						USART3
#define USART_Pin_Tx				GPIO_Pin_10
#define USART_Pin_Rx				GPIO_Pin_11
#define USART_GPIO					GPIOB
#define USARTx_IRQHandler			USART3_IRQHandler
#define USARTx_IRQn					USART3_IRQn
#define USART_RCC_INIT				do {	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);	\
											RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE); }\
									while(0)
#endif

void UART_Init();
void UART_SendChar(uint8_t data);
void UART_SendString(const char *str);
int UART_GetChar();

#ifdef USE_FREERTOS
int UART_GetCharBlocking();
#endif

#if (uartSIZE_OF_RING_BUFFER > 256)
#error "uartSIZE_OF_RING_BUFFER must be less then 256"
#endif
#endif
