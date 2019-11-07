/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "wifi.h"
#include "motor_control.h"

/* ϵͳʱ�����ú��� */
void SystemClock_Config(void);

/* ������ */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* ��ʼ�� */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
	delay_init(72);     /* use systick timer */
	
	/* �򿪴���1�Ľ����ж� */
	HAL_UART_Receive_IT(&huart1, (u8 *)aRxBuffer, RXBUFFERSIZE);   

	/* �򿪶�ʱ��2������ж� */
	HAL_TIM_Base_Start_IT(&htim2);
	
	/* wifiģ��ĳ�ʼ������
	 * ��Ҫ�ڴ�������֮����ܹ�����
	 * ������wifi�����ƣ�wifi���룬�˿ں�
	 */
	wifi_init("wireless_motor_v3.1", "abcd1234", "8123");	

	/* ��ѭ�� */
  while (1)
  {
		/* 
		 * �жϵ���Ƿ���Ҫ���򣬻���������ģʽ�������ź�����λ���ṩ��
		 * ģʽ1�����������õ��ֹͣ��Ȼ����ת����֮ǰ�ٶ�ֵ��
		 * ģʽ2��ֹͣ��һ�յ������źž�ֹͣ�����
		 * ģʽ�л�ͨ���궨��ı�־λ���������֡�
		 */
		if(my_motor.change_dir_flag == 1){
			my_motor.change_dir_flag = 0;
#if LIMIT_KEY_MODE
			motor_change_dir();			/* fore/back --> stop --> back/fore */
#else
			motor_stop();
#endif
		}
		
		/* �����ת�ĺ���ʵ�ֲ��֣�ͨ�����Ͳ�֮�����ʱ�������ٶȿ��� */
		if(my_motor.state != stop){
			
			/* �жϵ�ǰ״̬�����в�ͬ�Ĳ�����ѯ */
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
		 * �յ���λ���Ļ�ȡָ�����ѭ���з��͵����״̬��Ϣ
		 * �ñ�־λ�ڵ�����º����б���λ��	
		 */
		if(my_wifi.get_cmd_flag == 1){
			/* ��wifiģ��������18���ֽڵ���Ϣ */
			wifi_send_cmd("AT+CIPSEND=0,18");
			delay_ms(5);
			
			/* ��������״̬��Ϣ����18���ֽ� */
			printf("%1d,%2.2f,%10d", my_motor.state, my_motor.cur_speed.speed, my_motor.step_counter);
			
			/* ���״̬��־ */
			my_wifi.get_cmd_flag = 0;
			delay_ms(10);
			
			/* ��մ��ڽ��ձ�־ */
			USART_RX_STA = 0;
		}
 } /* while */
} /* main */


/* ����ϵͳʱ�� */
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

