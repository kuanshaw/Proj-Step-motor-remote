# 无线电机控制使用手册

[TOC]

## 功能描述

- PC连接WIFI模块；
- 

#### 限位开关

- 触发的时候，电机停止转动。

#### LED

- 电机状态LED灯：
  - 停止状态：LED长亮；
  - 运行状态：LED闪烁。
- WIFI状态LED灯：
  - 模块配置成功：LED长亮；
  - 模块配置不成功：LED快速闪烁；
  - 模块被连接成功：LED缓慢闪烁。
- 系统状态LED灯：
  - 正常运行：1秒闪烁两次。

#### WIFI模块

- WIFI模块作为AP服务器，供其他设备所连接。以下参数都可以更改。
- 服务器IP：192.168.4.1
- 端口号：8123
- SSID：wireless_motor
- 密码：abcd1234

## 引脚定义

- PC13   - LED_SYS(板载)
- PB10   - LED_MOTOR
- PB11   - LED_WIFI
- PB12   - MOTOR_A
- PB13   - MOTOR_B
- PB14   - MOTOR_C
- PB15   - MOTOR_D
- PB8     - LIMIT_KEY0
- PB9     - LIMIT_KEY1
- PA8     - WIFI_RST_PIN（低电平复位）
- PA9     - UART_TX（接目标板的RX） - 绿色
- PA10   - UART_RX（接目标板的TX） - 蓝色

## 上位机描述

- 

### 串口控制协议（选读）

- 可通过串口调试助手进行控制；

- 也可通过上位机进行控制。

## 下位机描述

### 软件框架



### 状态机

