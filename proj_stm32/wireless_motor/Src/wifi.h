#ifndef _WIFI_H
#define _WIFI_H
#include "stm32f1xx.h"

#define WIFI_LED_FLASHING_VAL 5					/* 5*20 = 100ms */
#define WIFI_DATA_OFFSET			12				/* 上位机发送回来的数据帧里边的偏置值 */
#define WIFI_BE_CONNECTED_OVERTIME 50


typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;


/* wifi状态 */
typedef enum{
	wifi_ok           = 0,
	wifi_nook         = 1,
	wifi_be_connected = 2,
} wifi_state_t;


/* wifi模块结构体 */
typedef struct{
	wifi_state_t state;
	const u8* ssid;		     	/* wifi name */
	const u8* password; 	  /* password */

	u16 led_counter;
	u16 led_flashing_val;		  
	u8 get_cmd_flag;
	
	u16 wifi_beconnected_overtime;
} wifi_t;

extern wifi_t my_wifi;


u8 wifi_send_cmd(u8 *cmd);
void wifi_send_data(u8 *data);

void wifi_init(u8* ssid, u8* password, u8* portnum);

void wifi_led_on();
void wifi_led_off();
void wifi_led_flashing();
void wifi_led_flashing_slow();
void wifi_led_flashing_fast();




#endif

