# 目录结构说明

```
doc # 项目文档
├───MicroSD
├───MUSIC
└───OLED
sd_files # sd卡存放文件
src # 项目源码
├───application # 应用驱动
│   └───music
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
│       │   ├───spi
│       │   │   └───spi_master
│       │   └───uart
│       └───stdio
├───drivers # 硬件驱动
│   ├───i2c
│   ├───i2s
│   ├───microSD
│   ├───OLED
│   ├───spi
│   └───wm8978
├───library # 第三方库
│   └───Fatfs
└───utilities # 工具
    └───crc
```