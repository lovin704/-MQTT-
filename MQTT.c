#include "stm32f4xx.h"                  // ����STM32F4ϵ�е��豸ͷ�ļ�
#include "MQTT.h"
#include "LOOP_BUFF.h"
#include "stdio.h"
#include "esp01_usart2.h"
#include "my_systick.h"

// �����ⲿ������ESP_RX_BUFΪѭ����������ESP_RX_CLIPΪ������Ƭ��
extern BufferTypeDef ESP_RX_BUF;
extern BufferClip ESP_RX_CLIP;
// �����ⲿ���ձ�־
extern uint8_t RecvFlag;

// ����MQTT�ṹ��ʵ������ʼ��Ϊ0
MQTT_t mqtt = {0};
// ����MQTT���ͻ�����
uint8_t MQTTSendbuff[BUFF_SIZE] = {0}; 

/*
    �����ݷ��͵�������
*/
void MQTTSendDataToBuff(void *data,uint32_t n,DataType_t dataType)
{
    if(data == NULL)  // �������Ϊ�գ�ֱ�ӷ���
        return;
    if(dataType == MQTT_INT)  // ������������ͣ�����ָ�뵽���ݵĵ�λ�ֽ�
        data = (uint8_t *)data + n - 1; 
    while(n--) {  // ѭ������������ֽڸ��Ƶ����ͻ�����
        *mqtt.sendBuffPointNow++ = *(uint8_t *)data;
        data = (uint8_t *)data + dataType;
    }
}

/*
    ����ʣ�೤�ȣ�������д�뻺����
*/
static void MakeMessageLength(void)
{
    uint8_t demp[4] = {0};  // ��ʱ�������ڴ洢ʣ�೤�ȵı���
    uint8_t length = 0;  // �����ĳ���
    uint8_t Byte = 0;  // ��ʱ�ֽڱ���
    uint32_t data = 0; // ���ݳ���

    // �������ݳ���
    data = mqtt.sendBuffPointNow - mqtt.sendBuff - 1;
    do
    {
        Byte = data % 128;  // ȡ��7λ
        data = data / 128;  // ����7λ
        if(data > 0) {  // �������ʣ������
            Byte = Byte | 128;  // �������λΪ1
        }
        demp[length] = Byte;  // �洢�������ֽ�
        length ++;
    }while(data > 0);

    // ��������ʣ�೤��д�뻺����
    while(length --)
    {
        *mqtt.sendBuff = demp[length];
        mqtt.sendBuff--;
    }
    // д����Ϣ����
    *mqtt.sendBuff = mqtt.messageType;
}

/* ������Ϣ��ʣ�೤�� */
uint32_t DecodeMessageLength(char* data)
{
    unsigned int m = 1;  // ����
    unsigned int value = 0;  // ������ʣ�೤��
    unsigned char Byte = 0;  // ��ʱ�ֽڱ���
    do
    {
        Byte = *data++;  // ��ȡһ���ֽ�
        value += (Byte & 0x7F) * m;  // �ۼӵ�7λ��ֵ
        m *= 128;  // ��������128
    } while ((Byte & 0x80) != 0);  // ������λΪ1��������ȡ
    printf("remainlentht: %d\n",value);  // ��ӡ������ʣ�೤��
    return value;
}

/*
    ��ʼ��MQTT
*/
void MQTTInit(void)
{
    mqtt.messageType = 0;  // ��ʼ����Ϣ����Ϊ0
    mqtt.sendBuff = MQTTSendbuff + 4;  // ��ʼ�����ͻ�����ָ��
    mqtt.sendBuffPointNow = MQTTSendbuff + 5;  // ��ʼ�����ͻ�������ǰָ��
}

/*
    MQTT����
*/
void MQTTConnect(MQTTConnectStruct_t *s1)
{
    MQTTInit();  // ����MQTTInit������ʼ��MQTT��ز���
    mqtt.messageType = MQTT_CONNECT;  // ������Ϣ����ΪMQTT_CONNECT����ʾ����һ������������Ϣ
    
    /***********************��װ�ɱ�ͷ******************************************/
    /*********************************
        @Э����= 4 + MQTT
        ����Ϊ �����ֽ� short���ͱ�ʾ����
    **********************************/
    
    uint16_t t = 0x4;  // Э��������Ϊ4����ΪЭ������"MQTT"
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // ����Э�������ȣ�ռ�������ֽ�
    MQTTSendDataToBuff("MQTT", 4, MQTT_CHAR);  // ����Э����"MQTT"
    
    // Э�鼶�� 0x4
    MQTTSendDataToBuff(&t, 1, MQTT_INT);  // ����Э�鼶��ֵΪ0x4
    
    // ���ӱ�־
    MQTTSendDataToBuff(&s1->connectFlag, 1, MQTT_INT);  // �������ӱ�־���ñ�־���������ӵ�һЩ������Ϣ
    
    // ���ֻʱ�䣬��Ϣ֮������ȴ�ʱ��
    MQTTSendDataToBuff(&s1->keepAliveTime, 2, MQTT_INT);  // ���ͱ���ʱ�䣬ռ�������ֽ�
    
    /***********************��װ��Ч�غ�******************************************/
    /*
        �ͻ��˱�ʶ�����û���������
        �ɱ�ͷ�����ӱ�־Ϊ0xc2
        1100 0010
    */
    // ��װ�ͻ��˱�ʶ�������� + ����
    t = strlen(s1->clientID);  // ����ͻ���ID�ĳ���
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // ���Ϳͻ���ID�ĳ���
    MQTTSendDataToBuff((void *)s1->clientID, t, MQTT_CHAR);  // ���Ϳͻ���ID
    
    // ��װ�û��������� + ����
    t = strlen(s1->userName);  // �����û����ĳ���
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // �����û����ĳ���
    MQTTSendDataToBuff((void *)s1->userName, t, MQTT_CHAR);  // �����û���
    
    // ��װ���룺���� + ����
    t = strlen(s1->password);  // ��������ĳ���
    MQTTSendDataToBuff(&t, 2, MQTT_INT);  // ��������ĳ���
    MQTTSendDataToBuff((void *)s1->password, t, MQTT_CHAR);  // ��������
    
    /*����ʣ�೤�ȣ�Э���� + ʣ�೤��*/
    MakeMessageLength();  // ����MakeMessageLength�������㲢��װʣ�೤��
    
    // ����MQTT��������
    Usart2_MqttSend(mqtt.sendBuff, mqtt.sendBuffPointNow - mqtt.sendBuff);
    Buffer_Print(&ESP_RX_BUF);  // ��ӡ���ջ����������ݣ����ڵ���
    My_delay_s(1);  // ��ʱ1�룬�ȴ���������Ӧ
    printf("CONNECT emqx again\n");  // �����ʾ��Ϣ����ʾ������Ҫ�ٴγ�������
    /*
    uint8_t data1, data2, data3, data4;
    uint8_t f = 0;
    do
    {
        printf("connect ack:\n");  // �����ʾ��Ϣ����ʾ���ڵȴ�������Ӧ
        My_delay_s(1);  // ��ʱ1��
        f++;  // ���Դ�����1
        Usart2_MqttSend(mqtt.sendBuff, mqtt.sendBuffPointNow - mqtt.sendBuff);  // �ٴη�����������
        Buffer_Print(&ESP_RX_BUF);  // ��ӡ���ջ����������ݣ����ڵ���
        if (f == 20) {  // ������Դ����ﵽ20��
            break;  // ����ѭ�����������ӳ���
        }
    } while (1);  // ����ѭ����ֱ���ﵽ����Դ���
    */
	printf("CONNECT EMQX OK\n");  // �����ʾ��Ϣ����ʾ�������
}

// ����MQTT����
void MQTTSub(MTTSubStruct_t * s1) 
{
    uint16_t temp = 0;
    MQTTInit();  // ��ʼ��MQTT
    // ������Ϣ����Ϊ��������
    mqtt.messageType = MQTT_SUBSCRIBE;
    /*�ɱ�ͷ*/
    // ��Ϣ��ʶ����16λ����
    MQTTSendDataToBuff(&s1->messageID, 2, MQTT_INT);
    /*��Ч�غ�*/
    temp = strlen(s1->payload);
    MQTTSendDataToBuff(&temp, 2, MQTT_INT);
    MQTTSendDataToBuff((void *)s1->payload, temp, MQTT_CHAR);

    /*���������ȼ�*/
    MQTTSendDataToBuff(&s1->QoS, 1, MQTT_INT);

    /*����ʣ�೤��*/
    MakeMessageLength();

    /*���Ͷ�������*/
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);

    // �ȴ����ĳɹ�ȷ��
    uint8_t data1,data2,data3,data4,data5;
    uint8_t t = 0;
    do
    {
        Buffer_Print(&ESP_RX_BUF);  // ��ӡ���ջ���������
        My_delay_s(2);  // ��ʱ2��
        t++;
        Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);  // ���·��Ͷ�������
        if(t == 3) {  // �������3����δ�ɹ����˳�ѭ��
            break;
        }
    }while((data1 != 0x90) && (data5 != 0x00));
    printf("subscrib ok\n");
    Buffer_Print(&ESP_RX_BUF);  // ��ӡ���ջ���������
}

// ����MQTT��Ϣ
void MQTTPublish(MTTPublishStruct_t *s1)
{
    uint16_t temp = 0;
    MQTTInit();  // ��ʼ��MQTT

    // ������Ϣ����Ϊ������Ϣ
    mqtt.messageType = MQTT_PUBLISH;
    mqtt.messageType |= s1->RETAN;
    mqtt.messageType |= s1->Qos << 1;
    mqtt.messageType |= s1->DUP << 3;

    /*�ɱ�ͷ: ���ⳤ�ȣ������ֽ�+������+��Ϣ��ʶ������ѡ+��Ϣ��ʶ��*/
    // ���ⳤ��
    temp = strlen(s1->topic);
    MQTTSendDataToBuff(&temp, 2, MQTT_INT);
    // ������
    MQTTSendDataToBuff((void *)s1->topic, temp, MQTT_CHAR);

    // ��Ϣ��ʶ������ѡ

    // ��Ч�غ�
    temp = strlen(s1->payload);
    MQTTSendDataToBuff((void *)s1->payload, temp, MQTT_CHAR);

    /*����ʣ�೤��*/
    MakeMessageLength();
    // ���ͷ�����Ϣ
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);
}

/*������������*/
void MQTTPing(void)
{
    unsigned short temp = 0;

    MQTTInit();  // ��ʼ��MQTT
    // ������Ϣ����Ϊ��������
    mqtt.messageType = MQTT_PINGREQ;
    /*����ʣ�೤��*/
    MakeMessageLength();
    /*������������*/
    Usart2_MqttSend(mqtt.sendBuff,mqtt.sendBuffPointNow - mqtt.sendBuff);
}