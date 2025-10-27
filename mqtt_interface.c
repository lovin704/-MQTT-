#include "stm32f4xx.h"                  // Device header
#include "mqtt_interface.h"
#include "MQTT.h"

/******************Э�����****************************************/
/*�ͻ��� �˺� ����*/
#define CLIENT_ID  "zhangyulong"
#define USER_NAME  " admin"
#define PASSWORD   "zyl20040704zyl"
#define PUBLISH_TOPIC "stm32/temp"
#define PUBLISH_PAYLOAD "zhangyulong like coffer "
//����������Ϣ
#define TOPIC_MESSAGE_ID 0xA
#define SUBSCRIBE_TOPIC1 "zidinghyi"
/******************************************************************/

/* MQTT���ӷ��������� */
void User_Connect(void)
{
    MQTTConnectStruct_t s1 = {0};
    
    s1.connectFlag = 0xC2; //���ӱ�־
    s1.keepAliveTime = MQTT_KEEP_ALIVE; //����ʱ��
    s1.clientID = CLIENT_ID;
    s1.password = PASSWORD;
    s1.userName = USER_NAME;
    
    MQTTConnect(&s1);
}
/* MQTT ���ĺ��� */
void User_Subscribe(void)
{
    MTTSubStruct_t s1 = {0};
    
    s1.messageID = TOPIC_MESSAGE_ID; //���ı�ʶ��
    s1.payload = SUBSCRIBE_TOPIC1; //��Ч�غ�
    s1.QoS = 0;
    MQTTSub(&s1);
}
/* MQTT �������� */
void User_Publish(void)
{
    MTTPublishStruct_t s1 = {0};
    s1.DUP = 0;
    s1.Qos = 0;
    s1.RETAN = 0;//����Ҫ����
    
    /***************************/
   // Tem = tem_arr[2] + tem_arr[3]/10;
    /****************************/
    //����
    s1.topic = PUBLISH_TOPIC;
    //s1.payload = &Tem;  //�¶�
    s1.payload = PUBLISH_PAYLOAD;
    MQTTPublish(&s1);
}

/* MQTT �������� */
void User_MqttPing(void)
{
    MQTTPing();
}