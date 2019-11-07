/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stdio.h"
	 
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
	 
#define UART_BAUD_RATE   115200				/* 串口1使用的波特率，115200 */
	 
/******************* uart1 receive buffer  *******************/
#define USART_REC_LEN  			50       /* uart1 receive buffer max length */
extern u8  USART_RX_BUF[USART_REC_LEN];  /* uart1 receive buffer */

extern u16 USART_RX_STA;              /* uart1 receive state */

#define RXBUFFERSIZE   1              /* uart1 receive buffer size */
extern u8 aRxBuffer[RXBUFFERSIZE];    /* uart1 receive buffer used by HAL Library */
	 
	 
extern UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */
