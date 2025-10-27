#include "stm32f4xx.h"                  // 包含STM32F4系列的设备头文件
#include "MQTT.h"
#include "LOOP_BUFF.h"
#include "stdio.h"
#include "esp01_usart2.h"
#include "my_systick.h"

// 声明外部变量，ESP_RX_BUF为循环缓冲区，ESP_RX_CLIP为缓冲区片段
extern BufferTypeDef ESP_RX_BUF;
extern BufferClip ESP_RX_CLIP;
// 声明外部接收标志
extern uint8_t RecvFlag;

// 定义MQTT结构体实例并初始化为0
MQTT_t mqtt = {0};
// 定义MQTT发送缓冲区
uint8_t MQTTSendbuff[BUFF_SIZE] = {0}; 

/*
    将数据发送到缓冲区
*/
void MQTTSendDataToBuff(void *data,uint32_t n,DataType_t dataType)
{
    if(data == NULL)  // 如果数据为空，直接返回
        return;
    if(dataType == MQTT_INT)  // 如果是整数类型，调整指针到数据的低位字节
        data = (uint8_t *)data + n - 1; 
    while(n--) {  // 循环将数据逐个字节复制到发送缓冲区
        *mqtt.sendBuffPointNow++ = *(uint8_t *)data;
        data = (uint8_t *)data + dataType;
    }
}

/*
    计算剩余长度，并将其写入缓冲区
*/
static void MakeMessageLength(void)
{
    uint8_t demp[4] = {0};  // 临时数组用于存储剩余长度的编码
    uint8_t length = 0;  // 编码后的长度
    uint8_t Byte = 0;  // 临时字节变量
    uint32_t data = 0; // 数据长度

    // 计算数据长度
    data = mqtt.sendBuffPointNow - mqtt.sendBuff - 1;
    do
    {
        Byte = data % 128;  // 取低7位
        data = data / 128;  // 右移7位
        if(data > 0) {  // 如果还有剩余数据
            Byte = Byte | 128;  // 设置最高位为1
        }
        demp[length] = Byte;  // 存储编码后的字节
        length ++;
    }while(data > 0);

    // 将编码后的剩余长度写入缓冲区
    while(length --)
    {
        *mqtt.sendBuff = demp[length];
        mqtt.sendBuff--;
    }
    // 写入消息类型
    *mqtt.sendBuff = mqtt.messageType;
}

/* 解码消息的剩余长度 */
uint32_t DecodeMessageLength(char* data)
{
    unsigned int m = 1;  // 乘数
    unsigned int value = 0;  // 解码后的剩余长度
    unsigned char Byte = 0;  // 临时字节变量
    do
    {
        Byte = *data++;  // 读取一个字节
        value += (Byte & 0x7F) * m;  // 累加低7位的值
        m *= 128;  // 乘数乘以128
    } while ((Byte & 0x80) != 0);  // 如果最高位为1，继续读取
    printf("remainlentht: %d\n",value);  // 打印解码后的剩余长度
    return value;
}

/*
    初始化MQTT
*/
void MQTTInit(void)
{
    mqtt.messageType = 0;  // 初始化消息类型为0
    mqtt.sendBuff = MQTTSendbuff + 4;  // 初始化发送缓冲区指针
    mqtt.sendBuffPointNow = MQTTSendbuff + 5;  // 初始化发送缓冲区当前指针
}

/*
    MQTT连接
*/
void MQTTConnect(MQTTConnectStruct_t *s1)
{
    MQTTInit();  // 调用MQTTInit函数初始化MQTT相关参数
    mqtt.messageType = MQTT_CONNECT;  // 设置消息类型为MQTT_CONNECT，表示这是一个连接请求消息
    
    /***********************封装可变头******************************************/
    /*********************************
        @协议名= 4 + MQTT
        长度为 两个字节 short类型表示长度
    **********************************/
    
    uint16_t t = 0x4;  // 协议名长度为4，因为协议名是"MQTT"
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // 发送协议名长度，占用两个字节
    MQTTSendDataToBuff("MQTT", 4, MQTT_CHAR);  // 发送协议名"MQTT"
    
    // 协议级别 0x4
    MQTTSendDataToBuff(&t, 1, MQTT_INT);  // 发送协议级别，值为0x4
    
    // 连接标志
    MQTTSendDataToBuff(&s1->connectFlag, 1, MQTT_INT);  // 发送连接标志，该标志包含了连接的一些配置信息
    
    // 保持活动时间，消息之间的最大等待时间
    MQTTSendDataToBuff(&s1->keepAliveTime, 2, MQTT_INT);  // 发送保活时间，占用两个字节
    
    /***********************封装有效载荷******************************************/
    /*
        客户端标识符，用户名，密码
        可变头中连接标志为0xc2
        1100 0010
    */
    // 封装客户端标识符：长度 + 内容
    t = strlen(s1->clientID);  // 计算客户端ID的长度
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // 发送客户端ID的长度
    MQTTSendDataToBuff((void *)s1->clientID, t, MQTT_CHAR);  // 发送客户端ID
    
    // 封装用户名：长度 + 内容
    t = strlen(s1->userName);  // 计算用户名的长度
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // 发送用户名的长度
    MQTTSendDataToBuff((void *)s1->userName, t, MQTT_CHAR);  // 发送用户名
    
    // 封装密码：长度 + 内容
    t = strlen(s1->password);  // 计算密码的长度
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // 发送密码的长度
    MQTTSendDataToBuff((void *)s1->password, t, MQTT_CHAR);  // 发送密码
    
    /*计算剩余长度，协议名 + 剩余长度*/
    MakeMessageLength();  // 调用MakeMessageLength函数计算并封装剩余长度
    
    // 发送MQTT连接请求
    Usart2_MqttSend(mqtt.sendBuff, mqtt.sendBuffPointNow - mqtt.sendBuff);
    Buffer_Print(&ESP_RX_BUF);  // 打印接收缓冲区的内容，用于调试
    My_delay_s(1);  // 延时1秒，等待服务器响应
    printf("CONNECT emqx again\n");  // 输出提示信息，表示可能需要再次尝试连接
    /*
    uint8_t data1, data2, data3, data4;
    uint8_t f = 0;
    do
    {
        printf("connect ack:\n");  // 输出提示信息，表示正在等待连接响应
        My_delay_s(1);  // 延时1秒
        f++;  // 尝试次数加1
        Usart2_MqttSend(mqtt.sendBuff, mqtt.sendBuffPointNow - mqtt.sendBuff);  // 再次发送连接请求
        Buffer_Print(&ESP_RX_BUF);  // 打印接收缓冲区的内容，用于调试
        if (f == 20) {  // 如果尝试次数达到20次
            break;  // 跳出循环，结束连接尝试
        }
    } while (1);  // 无限循环，直到达到最大尝试次数
    */
	printf("CONNECT EMQX OK\n");  // 输出提示信息，表示连接完成
}

// 订阅MQTT主题
void MQTTSub(MTTSubStruct_t * s1) 
{
    uint16_t temp = 0;
    MQTTInit();  // 初始化MQTT
    // 设置消息类型为订阅请求
    mqtt.messageType = MQTT_SUBSCRIBE;
    /*可变头*/
    // 消息标识符，16位整数
    MQTTSendDataToBuff(&s1->messageID, 2, MQTT_INT);
    /*有效载荷*/
    temp = strlen(s1->payload);
    MQTTSendDataToBuff(&temp, 2, MQTT_INT);
    MQTTSendDataToBuff((void *)s1->payload, temp, MQTT_CHAR);

    /*服务质量等级*/
    MQTTSendDataToBuff(&s1->QoS, 1, MQTT_INT);

    /*计算剩余长度*/
    MakeMessageLength();

    /*发送订阅请求*/
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);

    // 等待订阅成功确认
    uint8_t data1,data2,data3,data4,data5;
    uint8_t t = 0;
    do
    {
        Buffer_Print(&ESP_RX_BUF);  // 打印接收缓冲区内容
        My_delay_s(2);  // 延时2秒
        t++;
        Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);  // 重新发送订阅请求
        if(t == 3) {  // 如果尝试3次仍未成功，退出循环
            break;
        }
    }while((data1 != 0x90) && (data5 != 0x00));
    printf("subscrib ok\n");
    Buffer_Print(&ESP_RX_BUF);  // 打印接收缓冲区内容
}

// 发布MQTT消息
void MQTTPublish(MTTPublishStruct_t *s1)
{
    uint16_t temp = 0;
    MQTTInit();  // 初始化MQTT

    // 设置消息类型为发布消息
    mqtt.messageType = MQTT_PUBLISH;
    mqtt.messageType |= s1->RETAN;
    mqtt.messageType |= s1->Qos << 1;
    mqtt.messageType |= s1->DUP << 3;

    /*可变头: 主题长度，两个字节+主题名+消息标识符，可选+消息标识符*/
    // 主题长度
    temp = strlen(s1->topic);
    MQTTSendDataToBuff(&temp, 2, MQTT_INT);
    // 主题名
    MQTTSendDataToBuff((void *)s1->topic, temp, MQTT_CHAR);

    // 消息标识符，可选

    // 有效载荷
    temp = strlen(s1->payload);
    MQTTSendDataToBuff((void *)s1->payload, temp, MQTT_CHAR);

    /*计算剩余长度*/
    MakeMessageLength();
    // 发送发布消息
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);
}

/*发送心跳请求*/
void MQTTPing(void)
{
    unsigned short temp = 0;

    MQTTInit();  // 初始化MQTT
    // 设置消息类型为心跳请求
    mqtt.messageType = MQTT_PINGREQ;
    /*计算剩余长度*/
    MakeMessageLength();
    /*发送心跳请求*/
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);
}