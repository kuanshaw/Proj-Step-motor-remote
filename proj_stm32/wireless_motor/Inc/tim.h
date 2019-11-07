/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim2;

void MX_TIM2_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ tim_H */
