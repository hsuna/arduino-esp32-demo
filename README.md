# ESP32 Arduino 示例项目

本项目是一个基于 ESP32 和 Arduino 框架的综合示例工程，涵盖了基础外设控制、传感器读取、显示驱动及网络通信等多个模块。

## 项目结构

项目采用模块化设计，每个功能模块都封装在 `src/` 目录下的独立文件夹中，包含 `.cpp` 和 `.h` 文件，并使用命名空间进行隔离。

## 模块详细说明

### 1. OledTemp (温湿度显示 - 支持中文)

- **功能**: 读取 DHT11 传感器的温湿度数据，并在 128x64 OLED 屏幕上显示。支持中文界面。
- **硬件连接**:
  - OLED (I2C): SDA -> GPIO21, SCL -> GPIO22
  - DHT11: Data -> GPIO13
- **库依赖**: U8g2 (用于中文显示), DHT sensor library

### 2. Adc / Adc2 (模拟输入与输出)

- **功能**: 通过电位器控制 LED 亮度。
- **硬件连接**:
  - 电位器: 中间引脚 -> GPIO34
  - LED: GPIO2
- **区别**: `Adc` 使用标准的 `analogWrite`，`Adc2` 使用 ESP32 特有的 `ledc` 驱动。

### 3. Button (按键控制)

- **功能**: 通过按键切换 LED 状态，包含软件消抖逻辑。
- **硬件连接**:
  - 按键: GPIO12 (下拉输入)
  - LED: GPIO27

### 4. Exti (外部中断)

- **功能**: 使用硬件外部中断实现按键控制 LED。
- **硬件连接**:
  - 按键: GPIO14 (下拉输入)
  - LED: GPIO2

### 5. Led / Ledc / Pwm (LED 控制)

- **功能**:
  - `Led`: 红绿黄三色灯循环闪烁 (GPIO25, 27, 12)。
  - `Ledc`: 使用 `ledc` 实现呼吸灯效果 (GPIO27)。
  - `Pwm`: 使用 `analogWrite` 实现呼吸灯效果 (GPIO27)。

### 6. HeartBratTest (心跳检测)

- **功能**: 使用 KY-039 心率传感器进行实时心跳检测，包含简单的低通滤波算法。
- **硬件连接**:
  - 传感器信号: GPIO34

### 7. Timeout (定时器与 Ticker)

- **功能**: 演示硬件定时器 (`hw_timer_t`) 和 `Ticker` 库的使用，定时翻转 LED 状态。
- **硬件连接**:
  - LED: GPIO2, GPIO4, GPIO15

### 8. WifiTest (网络测试)

- **功能**: 连接指定 Wi-Fi 并向百度发送 HTTP GET 请求，在串口打印响应内容。
- **配置**: 需在 `WifiTest.cpp` 中修改 `SSID` 和 `PASSWORD`。

### 9. SmartHubTft (智能管家 - TFT 彩屏版)

- **功能**: 综合演示模块。在 1.54 寸 TFT (ST7789) 屏幕上显示环境数据（温湿度、光照）和系统状态。支持按键切换界面，RGB LED 根据温度显示不同颜色，光照超过阈值时蜂鸣器报警。
- **硬件连接**:
  - TFT (SPI): CS->5, DC->2, RST->15, SCL->18, SDA->23
  - DHT11: GPIO13
  - 光敏电阻 (LDR): GPIO35
  - 电位器 (阈值调节): GPIO34
  - 按键: GPIO14
  - 蜂鸣器: GPIO12
  - RGB LED: R->4, G->16, B->17
- **特点**: 使用 `U8g2_for_Adafruit_GFX` 实现 TFT 屏幕上的中文显示，并优化了刷新逻辑以消除闪烁。

## 常见问题与解决方案

### 库冲突：U8g2 与 U8g2_for_Adafruit_GFX

**问题描述**:
当同时安装 `U8g2` 和 `U8g2_for_Adafruit_GFX` 库时，编译会报错 `multiple definition of 'u8g2_font_...'`。这是因为两个库都包含了相同的字体定义文件 `u8g2_fonts.c`。

**解决方案**:

1. 将 `U8g2_for_Adafruit_GFX` 库从 `.pio/libdeps` 复制到项目的 `lib/` 目录下进行本地化管理。
2. 删除本地库中的 `lib/U8g2_for_Adafruit_GFX/src/u8g2_fonts.c` 文件。
3. 这样链接器会统一使用 `U8g2` 库中的字体定义，从而消除冲突。
4. 本项目已完成此处理，直接编译即可。

## 如何使用

1. **环境准备**: 安装 VS Code 和 PlatformIO 插件。
2. **配置敏感信息**:
   - 在 `src/` 目录下找到 `secrets.h.example` 文件。
   - 将其重命名为 `secrets.h`。
   - 在 `secrets.h` 中填写你的 Wi-Fi 名称 (`WIFI_SSID`) 和密码 (`WIFI_PASSWORD`)。
   - _注意：`secrets.h` 已被加入 `.gitignore`，不会被提交到仓库。_
3. **切换模块**: 在 `src/main.cpp` 中取消对应模块 `init()` 和 `update()` 函数的注释。
4. **编译上传**: 点击 PlatformIO 的 "Upload" 按钮。

## 依赖库

项目依赖已在 `platformio.ini` 中配置，主要包括：

- `U8g2`
- `U8g2_for_Adafruit_GFX`
- `DHT sensor library`
- `Adafruit Unified Sensor`
- `Adafruit GFX Library`
- `Adafruit ST7735 and ST7789 Library`
