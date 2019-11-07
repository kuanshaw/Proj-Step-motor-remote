/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

#include "usart.h"
#include "motor_control.h"


/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;


void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}


/* 定时器2的全局中断函数 */
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}


/* 串口1的全局中断函数 */
void USART1_IRQHandler(void)
{
	u32 timeout=0;
	u32 maxDelay=0x1FFFF;
	
  HAL_UART_IRQHandler(&huart1);

	timeout=0;
	while (HAL_UART_GetState(&huart1)!=HAL_UART_STATE_READY) //等待就绪
	{
		timeout++; 						//超时处理
		if(timeout>maxDelay) {
			break;		
		}
	}

	timeout=0;
	/* 一次处理完成之后，重新开启中断并设置RxXferCount为1 */ 
	while(HAL_UART_Receive_IT(&huart1,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)
	{
		timeout++; 					//超时处理
		if(timeout>maxDelay) break;	
	}
}
