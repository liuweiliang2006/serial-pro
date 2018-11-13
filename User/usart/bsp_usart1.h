#ifndef __USART_H
#define	__USART_H


#include "stm32f10x.h"
#include <stdio.h>

/*²¨ÌØÂÊ*/
#define  macUSART_BAUD_RATE           115200
#define  USE_USART1 

#ifdef USE_USART1
#define  macUSARTx                    USART1
#define  macUSART_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define  macUSART_CLK                 RCC_APB2Periph_USART1
#define  macUSART_GPIO_APBxClock_FUN  RCC_APB2PeriphClockCmd
#define  macUSART_GPIO_CLK            (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)     
#define  macUSART_TX_PORT             GPIOA   
#define  macUSART_TX_PIN              GPIO_Pin_9
#define  macUSART_RX_PORT             GPIOA   
#define  macUSART_RX_PIN              GPIO_Pin_10
#define  macUSART_IRQ                 USART1_IRQn
#define  macUSART_INT_FUN             USART1_IRQHandler
#endif

#ifdef USE_USART5
#define             macUSARTx                                UART5
#define             macUSART_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macUSART_CLK                             RCC_APB1Periph_UART5
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO)     
#define             macUSART_TX_PORT                         GPIOC   
#define             macUSART_TX_PIN                          GPIO_Pin_12
#define             macUSART_RX_PORT                         GPIOD 
#define             macUSART_RX_PIN                          GPIO_Pin_2
#define             macUSART_IRQ                             UART5_IRQn
#define             macUSART_INT_FUN                         UART5_IRQHandler
#endif


void USARTx_Config(void);
void NVIC_Configuration(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);


#endif /* __USART1_H */
