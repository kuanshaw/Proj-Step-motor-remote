#ifndef _MOTOR_CONTROL_H
#define _MOTOR_CONTROL_H

/******************* Include *******************/
#include "stm32f1xx_hal.h"
#include "usart.h"

/******************* motor parm   *******************/
#define MOTOR_ONE_ROUND_PULSE_NUM 2048			/* 转一圈需要的脉冲数 */
#define MOTOR_SPEED_BIAS 1									/* 速度误差，是的实时速度值小于设置的速度值 */

/* 速度控制最小单位，两个宏定义需要配套定义 */
#define MOTOR_SPEED_MIN_UINT          0.01    /* 0.1 | 0.01 | 0.001 | 0.0001 */
#define MOTOR_SPEED_COMMAND_MULTIPLE  100     /* 10  | 100  | 1000  | 10000  */

#define MOTOR_SPEED_MAX 99									/* 电机的最大转速 */
#define MOTOR_STEP_COUNTER_MAX 200000000		/* 计步器的最大值 */

/* 限位器处理模式 */
/* 0 - 停止 */
/* 1 - 换向 */
#define LIMIT_KEY_MODE 0  

/* LEd闪烁评率值 */
#define MOTOR_LED_FLASHING_VAL   35   /* 35*20 = 700ms */


/******************* Value *******************/

/* 电机状态 */
typedef enum {
	stop        = 0,
	fore_run    = 1,
	back_run    = 2,
}motor_state_t;


/* 电机速度结构体，包含速度以及延时值 */
typedef struct
{
	double speed;
	u32 delay_time_us;
}motor_speed_t;


/* 电机状态结构体 */
typedef struct
{
	motor_state_t  before_state;
	motor_state_t  state;
	motor_speed_t  before_preset_speed;
	motor_speed_t  preset_speed;   /* r/min */
	motor_speed_t  cur_speed;
	
	u8 change_dir_flag;
	u32 step_counter;
	
	u16 led_counter;
	u16 led_flashing_val;

} motor_t;

extern motor_t my_motor;

typedef void (*fp_void_led)();    /* pointer to function */
extern fp_void_led    state_led  [4];

/* 系统LED控制结构体 */
typedef struct{
	u16 led_counter;
	u16 led_flashing_val;		  /* val*10 ms = 50*10 = 500ms */
                            /* >> 250ms  */
														/* << 1000ms */
} sys_led_t;

extern sys_led_t sys_led;

/******************* Function *******************/

void motor_step(uint8_t flag);
void motor_stop();
void motor_pulse_run_fore();
void motor_pulse_run_back();
u32 speed_to_delaytime_us(double X);
double delaytime_us_to_speed(u32 T);

void motor_speed_up();
void motor_speed_down();
void motor_change_dir();
void motor_init();
void motor_step_counter_reset();
void motor_update();

void motor_led_flashing();
void motor_led_on();
void motor_led_off();


void sys_led_flashing();

int char_to_dec(u8 ch);

#endif
