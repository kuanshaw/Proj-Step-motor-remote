/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "wifi.h"
#include "motor_control.h"

/* 系统时钟配置函数 */
void SystemClock_Config(void);

/* 主函数 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* 初始化 */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
	delay_init(72);     /* use systick timer */
	
	/* 打开串口1的接收中断 */
	HAL_UART_Receive_IT(&huart1, (u8 *)aRxBuffer, RXBUFFERSIZE);   

	/* 打开定时器2的溢出中断 */
	HAL_TIM_Base_Start_IT(&htim2);
	
	/* wifi模块的初始化函数
	 * 需要在串口配置之后才能够调用
	 * 参数：wifi的名称，wifi密码，端口号
	 */
	wifi_init("wireless_motor_v3.1", "abcd1234", "8123");	

	/* 主循环 */
  while (1)
  {
		/* 
		 * 判断电机是否需要换向，换向有两种模式。换向信号由限位器提供。
		 * 模式1：换向，首先让电机停止，然后反向转动到之前速度值；
		 * 模式2：停止，一收到换向信号就停止电机。
		 * 模式切换通过宏定义的标志位来进行区分。
		 */
		if(my_motor.change_dir_flag == 1){
			my_motor.change_dir_flag = 0;
#if LIMIT_KEY_MODE
			motor_change_dir();			/* fore/back --> stop --> back/fore */
#else
			motor_stop();
#endif
		}
		
		/* 电机运转的函数实现部分，通过步和步之间的延时来进行速度控制 */
		if(my_motor.state != stop){
			
			/* 判断当前状态，运行不同的步数轮询 */
			if(my_motor.state == fore_run){					
				motor_pulse_run_fore();
				delay_us(my_motor.cur_speed.delay_time_us);
			}
			else if(my_motor.state == back_run){
				motor_pulse_run_back();
				delay_us(my_motor.cur_speed.delay_time_us);
			}
		}

		/* 
		 * 收到上位机的获取指令，在主循环中发送电机的状态信息
		 * 该标志位在电机更新函数中被置位。	
		 */
		if(my_wifi.get_cmd_flag == 1){
			/* 向wifi模块请求发送18个字节的信息 */
			wifi_send_cmd("AT+CIPSEND=0,18");
			delay_ms(5);
			
			/* 输出电机的状态信息，共18个字节 */
			printf("%1d,%2.2f,%10d", my_motor.state, my_motor.cur_speed.speed, my_motor.step_counter);
			
			/* 清空状态标志 */
			my_wifi.get_cmd_flag = 0;
			delay_ms(10);
			
			/* 清空串口接收标志 */
			USART_RX_STA = 0;
		}
 } /* while */
} /* main */


/* 配置系统时钟 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

