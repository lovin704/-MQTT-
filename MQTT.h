#ifndef _MQTT_H_
// 防止头文件被重复包含
#define _MQTT_H_

// 定义MQTT发送缓冲区的大小
#define BUFF_SIZE 512
// 定义MQTT连接的保活时间，单位为秒
#define MQTT_KEEP_ALIVE 300U

/***************************
    数据类型枚举
    @整数
    @字符串
    注意：大小端为小
****************************/
typedef enum
{
    MQTT_INT = -1, // 数据的单位是数据的低位字节
    MQTT_CHAR = 1,
}DataType_t;

// 定义MQTT消息类型的枚举
typedef enum
{
    MQTT_CONNECT = 0x10U,    // 客户端向服务器发起连接请求
    MQTT_CONNACK = 0x20U,   // 服务器对连接请求的响应
    MQTT_PUBLISH = 0x30U,   // 客户端向服务器发布消息
    MQTT_PUBACK = 0x40U,    // 服务器对QoS = 1的发布消息的确认
    MQTT_PUBREC = 0x50U,    // 服务器收到QoS = 2的发布消息的确认
    MQTT_PUBREL = 0x60U,    // 客户端对PUBREC消息的响应，用于释放消息
    MQTT_PUBCOMP = 0x70U,   // QoS = 2的消息发布完成确认
    MQTT_SUBSCRIBE = 0x82U, // 客户端向服务器订阅主题
    MQTT_SUBACK = 0x90U,    // 服务器对订阅请求的响应
    MQTT_UNSUBSCRIBE = 0xA2U,   // 客户端向服务器取消订阅主题
    MQTT_UNSUBACK = 0xB0U,  // 服务器对取消订阅请求的响应
    MQTT_PINGREQ = 0xC0U,   // 客户端向服务器发送心跳请求
    MQTT_PINGRESP = 0xD0U,  // 服务器对心跳请求的响应
    MQTT_DISCONNECT = 0xE0U // 客户端向服务器断开连接
}MessageType_t;

/*
    MQTT消息结构：固定头 + 可变头 + 有效载荷
*/
// 固定头结构体
typedef struct
{
    MessageType_t messageType; // 消息类型
    uint8_t *sendBuff;          // 发送缓冲区
    uint8_t *sendBuffPointNow;  // 指向发送缓冲区的当前位置
    uint32_t remainLength;  // 剩余长度
}MQTT_t;
// 连接结构体
typedef struct
{
    uint8_t connectFlag; // 连接标志
    uint16_t keepAliveTime; // 保活时间
    uint8_t *clientID;  // 客户端ID
    uint8_t *userName;  // 用户名
    uint8_t *password;  // 密码
}MQTTConnectStruct_t;
// 订阅结构体
typedef struct
{
    uint16_t messageID; // 消息ID
    uint8_t *payload; // 有效载荷
    uint8_t QoS; // 服务质量等级
}MTTSubStruct_t;
// 发布结构体
typedef struct
{
    uint8_t RETAN;
    uint8_t Qos;
    uint8_t DUP;
    uint16_t messageID; // 消息ID
    uint8_t * topic; // 主题
    uint8_t * payload;// 有效载荷
}MTTPublishStruct_t;

// 初始化MQTT相关参数
void MQTTInit(void);
// 建立MQTT连接
void MQTTConnect(MQTTConnectStruct_t *s1);
// 订阅MQTT主题
void MQTTSub(MTTSubStruct_t * s1);
// 发布MQTT消息
void MQTTPublish(MTTPublishStruct_t *s1);
// 发送MQTT心跳请求
void MQTTPing(void);
// 解码消息的剩余长度
uint32_t DecodeMessageLength(char* data);

#endif