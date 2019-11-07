#include "wifi.h"
#include "gpio.h"
#include "usart.h"
#include "delay.h"
#include "malloc.h"
#include "string.h" 


wifi_t my_wifi;   /* 定义wifi模块的信息结构体 */


/* wifi模块发送命令 */
/* 添加了换行 */
u8 wifi_send_cmd(u8 *cmd)
{
	u8 res=1;
	
	USART_RX_STA=0;
	printf("%s\r\n", cmd);

	return 1;
}


/* wifi模块发送数据 */
/* 未添加换行 */
void wifi_send_data(u8 *data)
{
	USART_RX_STA=0;
	printf("%s", data);
}


/* wifi模块的初始化 */
void wifi_init(u8* ssid, u8* password, u8* portnum){	
	u8 *p;
	p = mymalloc(SRAMIN, 100);
	
	my_wifi.state = wifi_nook;
	my_wifi.ssid = ssid;										/* wifi name */
	my_wifi.password = password; 	        	/* password */

	my_wifi.led_counter = 0;
	my_wifi.led_flashing_val = WIFI_LED_FLASHING_VAL;		  /* val*10 ms = 50*10 = 500ms */
																												/* >> 250ms  */
																												/* << 1000ms */
	my_wifi.get_cmd_flag = 0;

	my_wifi.wifi_beconnected_overtime = WIFI_BE_CONNECTED_OVERTIME;
	
//	while(1){
//		wifi_send_cmd("AT");
//		delay_ms(500);
//		if(USART_RX_STA & 0X8000){//接收到期待的应答结果
//			if(USART_RX_BUF[1]==0x4F){   /* OK */
//				USART_RX_STA=0;
//				break;
//			}
//		}
//	}
	
	WIFI_RST_PIN = 0;
	delay_ms(5000);			/* 延时5s，实现wifi模块的复位 */
	WIFI_RST_PIN = 1;
	
	wifi_send_cmd("AT+RST");  /* 软件复位 */
	delay_ms(100);
	wifi_send_cmd("AT+CWMODE=2");		/* 设置为AP模式的服务器 */
	delay_ms(100);
	wifi_send_cmd("AT+RST");  /* 软件复位 */
	delay_ms(100);
	
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4", my_wifi.ssid, my_wifi.password);   /* 设置wifi名称以及密码 */
	wifi_send_cmd(p);
	delay_ms(1000);
	
	wifi_send_cmd("AT+CIPMUX=1");
	delay_ms(100);
	sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);			/* 配置端口号 */
	wifi_send_cmd(p);
	delay_ms(100);
	
	my_wifi.state = wifi_ok;
	
	myfree(SRAMIN, p);
}

/* 以下为wifi模块的LED状态控制函数 */

void wifi_led_on(){
	LED_WIFI = 1;
}

void wifi_led_off(){
	LED_WIFI = 0;
}

void wifi_led_flashing_slow(){
	static u8 flag = 1;
	
	my_wifi.led_counter += 1;

	if((my_wifi.led_counter) > (my_wifi.led_flashing_val)<<1)
	{
		if(flag == 1){
			LED_WIFI = 1;
			flag = 0;
		}else if(flag == 0){
			LED_WIFI = 0;
			flag = 1;
		}
		my_wifi.led_counter = 0;
	}
}

void wifi_led_flashing(){
	static u8 flag = 1;
	
	my_wifi.led_counter += 1;

	if((my_wifi.led_counter) > (my_wifi.led_flashing_val))
	{
		if(flag == 1){
			LED_WIFI = 1;
			flag = 0;
		}else if(flag == 0){
			LED_WIFI = 0;
			flag = 1;
		}
		my_wifi.led_counter = 0;
	}
}

void wifi_led_flashing_fast(){
	static u8 flag = 1;
	
	my_wifi.led_counter += 1;

	if((my_wifi.led_counter) > (my_wifi.led_flashing_val)>>1)
	{
		if(flag == 1){
			LED_WIFI = 1;
			flag = 0;
		}else if(flag == 0){
			LED_WIFI = 0;
			flag = 1;
		}
		my_wifi.led_counter = 0;
	}
}

