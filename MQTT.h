#ifndef _MQTT_H_
// ��ֹͷ�ļ����ظ�����
#define _MQTT_H_

// ����MQTT���ͻ������Ĵ�С
#define BUFF_SIZE 512
// ����MQTT���ӵı���ʱ�䣬��λΪ��
#define MQTT_KEEP_ALIVE 300U

/***************************
    ��������ö��
    @����
    @�ַ���
    ע�⣺��С��ΪС
****************************/
typedef enum
{
    MQTT_INT = -1, // ���ݵĵ�λ�����ݵĵ�λ�ֽ�
    MQTT_CHAR = 1,
}DataType_t;

// ����MQTT��Ϣ���͵�ö��
typedef enum
{
    MQTT_CONNECT = 0x10U,    // �ͻ����������������������
    MQTT_CONNACK = 0x20U,   // �������������������Ӧ
    MQTT_PUBLISH = 0x30U,   // �ͻ����������������Ϣ
    MQTT_PUBACK = 0x40U,    // ��������QoS = 1�ķ�����Ϣ��ȷ��
    MQTT_PUBREC = 0x50U,    // �������յ�QoS = 2�ķ�����Ϣ��ȷ��
    MQTT_PUBREL = 0x60U,    // �ͻ��˶�PUBREC��Ϣ����Ӧ�������ͷ���Ϣ
    MQTT_PUBCOMP = 0x70U,   // QoS = 2����Ϣ�������ȷ��
    MQTT_SUBSCRIBE = 0x82U, // �ͻ������������������
    MQTT_SUBACK = 0x90U,    // �������Զ����������Ӧ
    MQTT_UNSUBSCRIBE = 0xA2U,   // �ͻ����������ȡ����������
    MQTT_UNSUBACK = 0xB0U,  // ��������ȡ�������������Ӧ
    MQTT_PINGREQ = 0xC0U,   // �ͻ����������������������
    MQTT_PINGRESP = 0xD0U,  // �������������������Ӧ
    MQTT_DISCONNECT = 0xE0U // �ͻ�����������Ͽ�����
}MessageType_t;

/*
    MQTT��Ϣ�ṹ���̶�ͷ + �ɱ�ͷ + ��Ч�غ�
*/
// �̶�ͷ�ṹ��
typedef struct
{
    MessageType_t messageType; // ��Ϣ����
    uint8_t *sendBuff;          // ���ͻ�����
    uint8_t *sendBuffPointNow;  // ָ���ͻ������ĵ�ǰλ��
    uint32_t remainLength;  // ʣ�೤��
}MQTT_t;
// ���ӽṹ��
typedef struct
{
    uint8_t connectFlag; // ���ӱ�־
    uint16_t keepAliveTime; // ����ʱ��
    uint8_t *clientID;  // �ͻ���ID
    uint8_t *userName;  // �û���
    uint8_t *password;  // ����
}MQTTConnectStruct_t;
// ���Ľṹ��
typedef struct
{
    uint16_t messageID; // ��ϢID
    uint8_t *payload; // ��Ч�غ�
    uint8_t QoS; // ���������ȼ�
}MTTSubStruct_t;
// �����ṹ��
typedef struct
{
    uint8_t RETAN;
    uint8_t Qos;
    uint8_t DUP;
    uint16_t messageID; // ��ϢID
    uint8_t * topic; // ����
    uint8_t * payload;// ��Ч�غ�
}MTTPublishStruct_t;

// ��ʼ��MQTT��ز���
void MQTTInit(void);
// ����MQTT����
void MQTTConnect(MQTTConnectStruct_t *s1);
// ����MQTT����
void MQTTSub(MTTSubStruct_t * s1);
// ����MQTT��Ϣ
void MQTTPublish(MTTPublishStruct_t *s1);
// ����MQTT��������
void MQTTPing(void);
// ������Ϣ��ʣ�೤��
uint32_t DecodeMessageLength(char* data);

#endif