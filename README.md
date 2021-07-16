# 目录结构说明

```
doc # 项目文档
├───MicroSD
└───OLED
src # 项目源码
├───config # 此目录下为mplab harmony v3 生成文件
│   └───default
│       ├───default.mhc
│       ├───peripheral
│       │   ├───clk
│       │   ├───coretimer
│       │   ├───evic
│       │   ├───gpio
│       │   ├───i2c
│       │   ├───spi
│       │   └───uart
│       └───stdio
├───drivers # 硬件驱动
│   ├───i2c
│   ├───microSD
│   ├───OLED
│   └───spi
├───library # 第三方库
│   └───Fatfs
└───utilities # 工具
    └───crc
```