/****************************** Include******************************/
#include "motor_control.h"
#include "gpio.h"
#include "tim.h"
#include "delay.h"
#include "wifi.h"
#include "malloc.h"

/****************************** Vaule ******************************/

motor_t my_motor;		/* 电机信息的结构体 */
sys_led_t sys_led;	/* 系统LED灯的结构体 */


/****************************** Function body ******************************/
/* 电机运转的元函数，四拍 */
void motor_step(uint8_t flag){
	switch(flag){
		case 0:
			MOTOR_A = 1;
			MOTOR_B = 0;
			MOTOR_C = 0;
			MOTOR_D = 0;
			break;
		case 1:
			MOTOR_A = 0;
			MOTOR_B = 1;
			MOTOR_C = 0;
			MOTOR_D = 0;
			break;
		case 2:
			MOTOR_A = 0;
			MOTOR_B = 0;
			MOTOR_C = 1;
			MOTOR_D = 0;
			break;
		case 3:
			MOTOR_A = 0;
			MOTOR_B = 0;
			MOTOR_C = 0;
			MOTOR_D = 1;
			break;
	}
	
	my_motor.step_counter++; 		/* 计步器加一 */
	
	/* 判断是否超过最大值 */
	if(my_motor.step_counter >= MOTOR_STEP_COUNTER_MAX){
		motor_step_counter_reset();
	}
}


/* 电机正转元函数 */
void motor_pulse_run_fore(){
	static uint8_t flag = 0;  /* 静态变量，实现四拍的循环 */
	
	my_motor.state = fore_run;
	
	motor_step(flag);
	
	flag++;
	if(flag == 4){
		flag = 0;
	}
}


/* 电机反转元函数 */
void motor_pulse_run_back(){
	static u8 flag = 4; 		/* 静态变量，实现四拍的循环 */
	
	my_motor.state = back_run;
	
	motor_step(flag - 1);
	flag--;
	
	if(flag == 0){
		flag = 4;
	}
}


/* 将速度值转换为延时的时间 */
u32 speed_to_delaytime_us(double X){
	if(X > MOTOR_SPEED_MAX){
		X = MOTOR_SPEED_MAX;
	}
	
	return (u32)(((60000000.0 / MOTOR_ONE_ROUND_PULSE_NUM) / X) + MOTOR_SPEED_BIAS);
}


/* 将延时时间转换为电机的实时速度 */
double delaytime_us_to_speed(u32 T){
	return (double)((60000000.0 / MOTOR_ONE_ROUND_PULSE_NUM) / T);
}


/* 电机停止函数 */
void motor_stop(){
	
	my_motor.state = stop;		/* 状态改为停止 */
	MOTOR_A = 0;
	MOTOR_B = 0;
	MOTOR_C = 0;
	MOTOR_D = 0;
	
	/* 清空速度值 */
	my_motor.cur_speed.delay_time_us = 0;
	my_motor.cur_speed.speed = 0;
	my_motor.preset_speed.speed = 0;
}


/* 加速函数，每次调用速度值增加一个速度单元，即0.01 */
/* 速度单元在宏定义中给出 */
void motor_speed_up(){
	my_motor.preset_speed.speed += MOTOR_SPEED_MIN_UINT;
}


/* 减速函数，每次调用速度值减少一个速度单元，即0.01 */
/* 速度单元在宏定义中给出 */
void motor_speed_down(){
	my_motor.preset_speed.speed -= MOTOR_SPEED_MIN_UINT;
}


/* 电机换向函数 */
/* 经历三个状态：反/正转 - 停止 - 正/反转 */
void motor_change_dir(){
	/* 保存速度参数 */
	my_motor.before_state = my_motor.state;
	my_motor.before_preset_speed = my_motor.preset_speed;
	
	motor_stop();    /* stop */
	
	my_motor.state = my_motor.before_state;

	/* 根据之前的状态再进行判断 */
	if(my_motor.state != back_run){
		my_motor.state = back_run;
	}
	else if(my_motor.state != fore_run){
		my_motor.state = fore_run;
	}
	
	/* 设置速度 */
	my_motor.preset_speed = my_motor.before_preset_speed;	
}


/* 电机初始化，将一些信息配置为初始化状态 */
void motor_init(){
	my_motor.preset_speed.speed = 0;
	my_motor.preset_speed.delay_time_us = 0;
	my_motor.cur_speed.speed = 0;
	my_motor.cur_speed.delay_time_us = 0;
	my_motor.state = stop;
	my_motor.step_counter = 0;
	my_motor.change_dir_flag = 0;
	
	my_motor.led_counter = 0;
	my_motor.led_flashing_val = MOTOR_LED_FLASHING_VAL;
}


/* 电机的计步器步数重置函数，即清零 */
void motor_step_counter_reset(){
	my_motor.step_counter = 0;
}

u8 len, rx_p;

/* 电机状态更新函数，每一个定时器中断更新一次 */
/* 在定时器回调函数中调用 */
void motor_update(){

	int cmd_speed_i, cmd_speed_d;
	u8 *farme_p;
	u8 is_wifi_data;
	
	farme_p = mymalloc(SRAMIN, 100);
	
	u8 limit_key0, limit_key1;
	static u8 time_count = 0;
	
	/* 读取限位器的值 */
	limit_key0 = LIMIT_KEY0;
	limit_key1 = LIMIT_KEY1;
	
	/* 判断是否触发了限位器 */
	if(limit_key0 == 0 || limit_key1 == 0){
		if(my_motor.state != stop){
			my_motor.change_dir_flag = 1;		/* 标志位置1，在主循环中处理 */
		}
	}

	/* 接受到了来自上位机的信息 */
	if(USART_RX_STA & 0x8000)
	{
		len = USART_RX_STA&0x3fff;           /* 获取长度 */
	
		rx_p= 0 ;
		is_wifi_data = 0;

		/* 判断是不是数据帧 */
		if(USART_RX_BUF[2] == '+')
		{
			/* 如果是A类指令 */
			if(USART_RX_BUF[WIFI_DATA_OFFSET] == 'A'){		
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){		/* AA，保持连接指令，上位机固定周期发送过来的指令，保持两者连接 */
					case 'A':
						/* 保持连接状态 */
						my_wifi.state = wifi_be_connected;
						/* 更新超时时间，避免连接超时 */
						my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
						break;
				}
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){		/* AB，上位机获取电机状态的指令， */
					case 'B':
						if(my_wifi.get_cmd_flag == 0){
							my_wifi.get_cmd_flag = 1;			/* 将标志位置1，在主循环中处理 */
						}
						/* 保持连接状态 */
						my_wifi.state = wifi_be_connected;
						/* 更新超时时间，避免连接超时 */
						my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
						break;
				}
			}
			
			/* 收到了B类指令 */
			else if(USART_RX_BUF[WIFI_DATA_OFFSET] == 'B'){
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){
					case '0':																	/* B0，设置电机的正反转和速度值 */
						/* 更新正反转状态 */
						my_motor.state = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET+2]);      /* stop-0; fore_run-1; back_run-2 */

					  /* 计算速度值 */
						cmd_speed_i = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 3])*10 + char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 4]);
						cmd_speed_d = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 6])*10 + char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 7]);
						/*更新速度值*/
					  my_motor.preset_speed.speed = cmd_speed_i + ((double)cmd_speed_d/100);
					
						break;
					case '1':																	/* B1，计步器重置指令 */
						motor_step_counter_reset();
						break;	
					case '2':																	/* B2，电机停止指令 */
						motor_stop();
						break;
					case '3':																	/* B3，加速指令 */
						motor_speed_up();
						break;
					case '4':																	/* B4，减速指令 */
						motor_speed_down();
						break;
					case '5':   															/* B5，换向指令*/
						if(my_motor.state != stop){
							motor_change_dir();
						}
						break;
				}
			}
		}
		USART_RX_STA = 0;          /* Clear receive State */
	}
	
	/* wifi连接超时处理 */
	my_wifi.wifi_beconnected_overtime--;
	if(my_wifi.wifi_beconnected_overtime == 0){
		my_wifi.state = wifi_nook;		/* 更新状态 */
		motor_stop();						/* 停止电机 */
		my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
	}
	
	/* 根据速度值计算延时的时间，计算实时的速度值 */
	if(my_motor.state != stop){
		my_motor.cur_speed.delay_time_us = speed_to_delaytime_us(my_motor.preset_speed.speed); /* Update the delaytime */
		my_motor.cur_speed.speed = delaytime_us_to_speed(my_motor.cur_speed.delay_time_us);    /* Caulate the speed */ 
	}

	/* wifi模块的LED控制 */
	/* 连接状态 - LED亮 */
	/* 未连接状态 - LED闪烁 */
	if(my_wifi.state == wifi_be_connected){
		wifi_led_on();
	}else{
		wifi_led_flashing();
	}
	
	/* 电机的LED控制 */
	/* 停止状态 - LED亮 */
	/* 运转状态 - LED暗 */
	if(my_motor.state == stop){
		motor_led_off();
	}else{
		motor_led_on();
	}
	
	/* 系统LED保持闪烁 */
	sys_led_flashing();
}


/* 以下LED控制函数 */

void sys_led_flashing(){
	static u8 flag = 1;
	
	sys_led.led_counter += 1;

	if((sys_led.led_counter) > 100 )    /* 1s */
	{
		if(flag == 1){
			LED_SYS = 1;
			flag = 0;
		}else if(flag == 0){
			LED_SYS = 0;
			flag = 1;
		}
		sys_led.led_counter = 0;
	}
}


void motor_led_on(){
	LED_MOTOR = 1;
}


void motor_led_off(){
	LED_MOTOR = 0;
}


void motor_led_flashing(){
	static u8 flag = 1;
	
	my_motor.led_counter += 1;

	if((my_motor.led_counter) > (my_motor.led_flashing_val))
	{
		if(flag == 1){
			LED_MOTOR = 1;
			flag = 0;
		}else if(flag == 0){
			LED_MOTOR = 0;
			flag = 1;
		}
		my_motor.led_counter = 0;
	}
}


/* 字符转成十进制 */
int char_to_dec(u8 ch){
	int val = 0;

	if((ch >= '0') && (ch <= '9')){
		val = ch-'0';
	}

	return val;
}

/* end of file */
