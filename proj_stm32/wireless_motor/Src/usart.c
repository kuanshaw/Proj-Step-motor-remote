/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "gpio.h"
#include "stdio.h"	

struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

void _sys_exit(int x) 
{ 
	x = x; 
} 

/* 重新定义printf函数 */
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);
	USART1->DR=(u8)ch;      
	return ch;
}

u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA = 0;       //接收状态标记	

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲

UART_HandleTypeDef huart1;	/* 串口1的句柄 */


/* 串口初始化函数 */
void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = UART_BAUD_RATE;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART1)
  {
    /* Peripheral clock enable */
    __USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  }
} 


/* 串口接受中断的回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* 串口1的中断 */
	/* 串口1的数据必须要以EF为结尾 */
	if(huart->Instance==USART1)
	{
		if((USART_RX_STA&0x8000)==0)		//接收未完成
		{
			if(USART_RX_STA&0x4000)				//接收到了'E'
			{
				if(aRxBuffer[0] != 'F')USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成 
			}
			else //还没收到'E'
			{	
				if(aRxBuffer[0] == 'E') {
					USART_RX_STA|=0x4000;
				}
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=(u8)aRxBuffer[0] ; /* 保存到缓存区 */
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1)){
						USART_RX_STA=0;    //接收数据错误,重新开始接收	  
					}
				}		 
			}
		}
	} /* if uart1 */
}

