/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
void MX_GPIO_Init(void);
#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOB_IDR_Addr (GPIOB_BASE+8)  //0x40010C08

#define GPIOA_ODR_Addr (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE+12) //0x4001100C

#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)

/* 初始化所有使用的GPIO引脚 */
#define LED_SYS     PCout(13)
#define LED_MOTOR   PBout(10)
#define LED_WIFI    PBout(11)

#define MOTOR_A   PBout(12)
#define MOTOR_B   PBout(13)
#define MOTOR_C   PBout(14)
#define MOTOR_D   PBout(15)

#define LIMIT_KEY0  PBin(8)
#define LIMIT_KEY1  PBin(9)

#define WIFI_RST_PIN  PAout(8)
