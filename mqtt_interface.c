#include "stm32f4xx.h"                  // Device header
#include "mqtt_interface.h"
#include "MQTT.h"

/******************协议相关****************************************/
/*客户端 账号 密码*/
#define CLIENT_ID  "zhangyulong"
#define USER_NAME  " admin"
#define PASSWORD   "zyl20040704zyl"
#define PUBLISH_TOPIC "stm32/temp"
#define PUBLISH_PAYLOAD "zhangyulong like coffer "
//订阅主题消息
#define TOPIC_MESSAGE_ID 0xA
#define SUBSCRIBE_TOPIC1 "zidinghyi"
/******************************************************************/

/* MQTT连接服务器函数 */
void User_Connect(void)
{
    MQTTConnectStruct_t s1 = {0};
    
    s1.connectFlag = 0xC2; //连接标志
    s1.keepAliveTime = MQTT_KEEP_ALIVE; //保活时间
    s1.clientID = CLIENT_ID;
    s1.password = PASSWORD;
    s1.userName = USER_NAME;
    
    MQTTConnect(&s1);
}
/* MQTT 订阅函数 */
void User_Subscribe(void)
{
    MTTSubStruct_t s1 = {0};
    
    s1.messageID = TOPIC_MESSAGE_ID; //报文标识符
    s1.payload = SUBSCRIBE_TOPIC1; //有效载荷
    s1.QoS = 0;
    MQTTSub(&s1);
}
/* MQTT 发布函数 */
void User_Publish(void)
{
    MTTPublishStruct_t s1 = {0};
    s1.DUP = 0;
    s1.Qos = 0;
    s1.RETAN = 0;//不需要保留
    
    /***************************/
   // Tem = tem_arr[2] + tem_arr[3]/10;
    /****************************/
    //主题
    s1.topic = PUBLISH_TOPIC;
    //s1.payload = &Tem;  //温度
    s1.payload = PUBLISH_PAYLOAD;
    MQTTPublish(&s1);
}

/* MQTT 心跳函数 */
void User_MqttPing(void)
{
    MQTTPing();
}