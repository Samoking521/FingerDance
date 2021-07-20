# 目录结构说明

```
doc # 项目文档
├───MicroSD
├───MUSIC
└───OLED
sd_files # sd卡存放文件
src # 项目源码
├───application # 程序应用
│   ├───music
│   └───play_mode
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
│   ├───microSD
│   ├───OLED
│   ├───spi
│   ├───wm8978
│   └───ws2812b
├───library # 第三方库
│   └───Fatfs
└───utilities # 工具
    └───crc
```