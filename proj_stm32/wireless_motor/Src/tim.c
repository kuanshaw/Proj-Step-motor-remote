
/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "motor_control.h"

TIM_HandleTypeDef htim2;			/* 定时器2的句柄，在.h中有申明外部 */

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 720-1;							/* 初始值 */
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2000-1;								/* 预分频率值，定时器触发时间为720*2000/720000 = 20ms*/
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);
	

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM2)
  {
    /* Peripheral clock enable */
    __TIM2_CLK_ENABLE();

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

  }
}


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM2)
  {
    /* Peripheral clock disable */
    __TIM2_CLK_DISABLE();

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(TIM2_IRQn);

  }
} 

/* 定时器中断回调函数，所有的定时器中断在这里边处理，
   根据if条件语句进行区分是哪个定时器产生了中断 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	
	/* 定时器2来更新电机的状态参数 */
	if(htim==(&htim2)){
		motor_update();			
	}
}
