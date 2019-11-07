/****************************** Include******************************/
#include "motor_control.h"
#include "gpio.h"
#include "tim.h"
#include "delay.h"
#include "wifi.h"
#include "malloc.h"

/****************************** Vaule ******************************/

motor_t my_motor;		/* �����Ϣ�Ľṹ�� */
sys_led_t sys_led;	/* ϵͳLED�ƵĽṹ�� */


/****************************** Function body ******************************/
/* �����ת��Ԫ���������� */
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
	
	my_motor.step_counter++; 		/* �Ʋ�����һ */
	
	/* �ж��Ƿ񳬹����ֵ */
	if(my_motor.step_counter >= MOTOR_STEP_COUNTER_MAX){
		motor_step_counter_reset();
	}
}


/* �����תԪ���� */
void motor_pulse_run_fore(){
	static uint8_t flag = 0;  /* ��̬������ʵ�����ĵ�ѭ�� */
	
	my_motor.state = fore_run;
	
	motor_step(flag);
	
	flag++;
	if(flag == 4){
		flag = 0;
	}
}


/* �����תԪ���� */
void motor_pulse_run_back(){
	static u8 flag = 4; 		/* ��̬������ʵ�����ĵ�ѭ�� */
	
	my_motor.state = back_run;
	
	motor_step(flag - 1);
	flag--;
	
	if(flag == 0){
		flag = 4;
	}
}


/* ���ٶ�ֵת��Ϊ��ʱ��ʱ�� */
u32 speed_to_delaytime_us(double X){
	if(X > MOTOR_SPEED_MAX){
		X = MOTOR_SPEED_MAX;
	}
	
	return (u32)(((60000000.0 / MOTOR_ONE_ROUND_PULSE_NUM) / X) + MOTOR_SPEED_BIAS);
}


/* ����ʱʱ��ת��Ϊ�����ʵʱ�ٶ� */
double delaytime_us_to_speed(u32 T){
	return (double)((60000000.0 / MOTOR_ONE_ROUND_PULSE_NUM) / T);
}


/* ���ֹͣ���� */
void motor_stop(){
	
	my_motor.state = stop;		/* ״̬��Ϊֹͣ */
	MOTOR_A = 0;
	MOTOR_B = 0;
	MOTOR_C = 0;
	MOTOR_D = 0;
	
	/* ����ٶ�ֵ */
	my_motor.cur_speed.delay_time_us = 0;
	my_motor.cur_speed.speed = 0;
	my_motor.preset_speed.speed = 0;
}


/* ���ٺ�����ÿ�ε����ٶ�ֵ����һ���ٶȵ�Ԫ����0.01 */
/* �ٶȵ�Ԫ�ں궨���и��� */
void motor_speed_up(){
	my_motor.preset_speed.speed += MOTOR_SPEED_MIN_UINT;
}


/* ���ٺ�����ÿ�ε����ٶ�ֵ����һ���ٶȵ�Ԫ����0.01 */
/* �ٶȵ�Ԫ�ں궨���и��� */
void motor_speed_down(){
	my_motor.preset_speed.speed -= MOTOR_SPEED_MIN_UINT;
}


/* ��������� */
/* ��������״̬����/��ת - ֹͣ - ��/��ת */
void motor_change_dir(){
	/* �����ٶȲ��� */
	my_motor.before_state = my_motor.state;
	my_motor.before_preset_speed = my_motor.preset_speed;
	
	motor_stop();    /* stop */
	
	my_motor.state = my_motor.before_state;

	/* ����֮ǰ��״̬�ٽ����ж� */
	if(my_motor.state != back_run){
		my_motor.state = back_run;
	}
	else if(my_motor.state != fore_run){
		my_motor.state = fore_run;
	}
	
	/* �����ٶ� */
	my_motor.preset_speed = my_motor.before_preset_speed;	
}


/* �����ʼ������һЩ��Ϣ����Ϊ��ʼ��״̬ */
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


/* ����ļƲ����������ú����������� */
void motor_step_counter_reset(){
	my_motor.step_counter = 0;
}

u8 len, rx_p;

/* ���״̬���º�����ÿһ����ʱ���жϸ���һ�� */
/* �ڶ�ʱ���ص������е��� */
void motor_update(){

	int cmd_speed_i, cmd_speed_d;
	u8 *farme_p;
	u8 is_wifi_data;
	
	farme_p = mymalloc(SRAMIN, 100);
	
	u8 limit_key0, limit_key1;
	static u8 time_count = 0;
	
	/* ��ȡ��λ����ֵ */
	limit_key0 = LIMIT_KEY0;
	limit_key1 = LIMIT_KEY1;
	
	/* �ж��Ƿ񴥷�����λ�� */
	if(limit_key0 == 0 || limit_key1 == 0){
		if(my_motor.state != stop){
			my_motor.change_dir_flag = 1;		/* ��־λ��1������ѭ���д��� */
		}
	}

	/* ���ܵ���������λ������Ϣ */
	if(USART_RX_STA & 0x8000)
	{
		len = USART_RX_STA&0x3fff;           /* ��ȡ���� */
	
		rx_p= 0 ;
		is_wifi_data = 0;

		/* �ж��ǲ�������֡ */
		if(USART_RX_BUF[2] == '+')
		{
			/* �����A��ָ�� */
			if(USART_RX_BUF[WIFI_DATA_OFFSET] == 'A'){		
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){		/* AA����������ָ���λ���̶����ڷ��͹�����ָ������������� */
					case 'A':
						/* ��������״̬ */
						my_wifi.state = wifi_be_connected;
						/* ���³�ʱʱ�䣬�������ӳ�ʱ */
						my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
						break;
				}
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){		/* AB����λ����ȡ���״̬��ָ� */
					case 'B':
						if(my_wifi.get_cmd_flag == 0){
							my_wifi.get_cmd_flag = 1;			/* ����־λ��1������ѭ���д��� */
						}
						/* ��������״̬ */
						my_wifi.state = wifi_be_connected;
						/* ���³�ʱʱ�䣬�������ӳ�ʱ */
						my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
						break;
				}
			}
			
			/* �յ���B��ָ�� */
			else if(USART_RX_BUF[WIFI_DATA_OFFSET] == 'B'){
				switch(USART_RX_BUF[WIFI_DATA_OFFSET+1]){
					case '0':																	/* B0�����õ��������ת���ٶ�ֵ */
						/* ��������ת״̬ */
						my_motor.state = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET+2]);      /* stop-0; fore_run-1; back_run-2 */

					  /* �����ٶ�ֵ */
						cmd_speed_i = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 3])*10 + char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 4]);
						cmd_speed_d = char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 6])*10 + char_to_dec(USART_RX_BUF[WIFI_DATA_OFFSET + 7]);
						/*�����ٶ�ֵ*/
					  my_motor.preset_speed.speed = cmd_speed_i + ((double)cmd_speed_d/100);
					
						break;
					case '1':																	/* B1���Ʋ�������ָ�� */
						motor_step_counter_reset();
						break;	
					case '2':																	/* B2�����ָֹͣ�� */
						motor_stop();
						break;
					case '3':																	/* B3������ָ�� */
						motor_speed_up();
						break;
					case '4':																	/* B4������ָ�� */
						motor_speed_down();
						break;
					case '5':   															/* B5������ָ��*/
						if(my_motor.state != stop){
							motor_change_dir();
						}
						break;
				}
			}
		}
		USART_RX_STA = 0;          /* Clear receive State */
	}
	
	/* wifi���ӳ�ʱ���� */
	my_wifi.wifi_beconnected_overtime--;
	if(my_wifi.wifi_beconnected_overtime == 0){
		my_wifi.state = wifi_nook;		/* ����״̬ */
		motor_stop();						/* ֹͣ��� */
		my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
	}
	
	/* �����ٶ�ֵ������ʱ��ʱ�䣬����ʵʱ���ٶ�ֵ */
	if(my_motor.state != stop){
		my_motor.cur_speed.delay_time_us = speed_to_delaytime_us(my_motor.preset_speed.speed); /* Update the delaytime */
		my_motor.cur_speed.speed = delaytime_us_to_speed(my_motor.cur_speed.delay_time_us);    /* Caulate the speed */ 
	}

	/* wifiģ���LED���� */
	/* ����״̬ - LED�� */
	/* δ����״̬ - LED��˸ */
	if(my_wifi.state == wifi_be_connected){
		wifi_led_on();
	}else{
		wifi_led_flashing();
	}
	
	/* �����LED���� */
	/* ֹͣ״̬ - LED�� */
	/* ��ת״̬ - LED�� */
	if(my_motor.state == stop){
		motor_led_off();
	}else{
		motor_led_on();
	}
	
	/* ϵͳLED������˸ */
	sys_led_flashing();
}


/* ����LED���ƺ��� */

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


/* �ַ�ת��ʮ���� */
int char_to_dec(u8 ch){
	int val = 0;

	if((ch >= '0') && (ch <= '9')){
		val = ch-'0';
	}

	return val;
}

/* end of file */
