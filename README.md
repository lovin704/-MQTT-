# -MQTT-
基于 STM32 构建物联网数据采集系统，集成 DHT11 与 SRF05 实现温湿度、距离实时监测；采用双串口架 构（USART1 连上位机、USART2 配 ESP01）建 WiFi 连接，结合环形缓冲区保障每秒 1000 + 字节数据流无丢失；基 于 FreeRTOS 实现多任务调度，管理传感器采样、数据处理、MQTT 通信，每 5 秒稳定上传数据至 EMQX 服务器；支 持双向通信，可通过 MQTT 订阅接收指令，具备断线重连与消息持久化机制
技术栈：STM32F407 微控制器、FreeRTOS、MQTT 协议、USART 串口、GPIO 接口、TIM 定时器、AT 指令集、
ESP01 模块、C 语言、DHT11 温湿度传感器、SRF05 超声波传感器、OLED 显示屏。
关键实现：传感器驱动：精准时序控制实现 DHT11 数据读取，用定时器与外部中断测超声波往返时间算距离；串口通信
驱动：配置 USART1/2 波特率、数据位等参数，实现数据收发；环形缓冲区：通过 Buffer_Push/Pop 完成数据读写，支
持循环存储；MQTT 协议：用 MQTTConnect/MQTTPublish/MQTTSub 实现连接、发布、订阅与消息封装发送；
FreeRTOS 任务管理：xTaskCreate 创建采集、通信等任务，vTaskStartScheduler 启动调度。
项目成果: 实现多传感器数据采集处理，实时获取温湿度、距离信息；WiFi 连接 EMQX 服务器完成实时数据传输；
FreeRTOS 提升系统并发处理能力与稳定性；环形缓冲区避免串口数据丢失溢出；系统可扩展性、可维护性良好，便于后
续扩展传感器与功能。
