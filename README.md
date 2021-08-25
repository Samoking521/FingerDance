# FingerDance

## Description

本项目是一个在嵌入式平台上完成的音乐游戏机。游戏机以Microchip的PIC32MX270F256D单片机为核心，并由LED灯带(WS2812B)，音响，OLED，MicroSD，机械按键等外设组成。游戏时灯带上的LED随着音乐的节奏流动，玩家需要在LED流动到指定位置时按下对应的按键，游戏以按键时机的准确度作为游戏的评价指标。

## Used

- Peripheral Modules
    - OC
    - Timer
    - I2C
    - I2S
    - SPI
    - DMA
    - Core Timer
- Hardware
    - PIC32MX270F256D
    - WM8978
    - WS2812B
    - 1.3inch OLED
    - MicroSD
    - Key
    - Audio with 3.5mm interface
- Software
    - MPLAB X IDE
    - MPLAB XC32 Compiler
    - MPLAB Harmony v3

## Demonstrate

<image src="images/image.png">

## Comments

详细设计文档参考[DesignReport](DesignReport.html)

## Contents

```
doc # 项目文档
├───MicroSD
├───MUSIC
├───OLED
└───WS2812B
src # 项目源码
├───application # 程序应用
│   ├───game_mode
│   ├───music
│   ├───play_mode
│   ├───pre_mode
│   ├───screen
│   ├───sdfile
│   └───startup_mode
├───config # 此目录下为mplab harmony v3 生成文件
│   └───default
│       ├───audio
│       │   └───peripheral
│       │       └───i2s
│       ├───default.mhc
│       ├───peripheral
│       │   ├───clk
│       │   ├───coretimer
│       │   ├───dmac
│       │   ├───evic
│       │   ├───gpio
│       │   ├───i2c
│       │   │   └───master
│       │   ├───ocmp
│       │   ├───spi
│       │   │   └───spi_master
│       │   ├───tmr
│       │   └───uart
│       └───stdio
├───drivers # 硬件驱动
│   ├───i2c
│   ├───i2s
│   ├───key
│   ├───microSD
│   ├───OLED
│   ├───spi
│   ├───wm8978
│   └───ws2812b
├───library # 第三方库
│   └───Fatfs
└───utilities # 工具
    ├───ans_queue
    ├───crc
    └───led_queue
```