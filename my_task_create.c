#include "stm32f4xx.h"                  // Device header
#include "my_freertos_fun.h"
#include "stdio.h"
#include "my_dht11.h"
#include "my_srf05.h"
#include "esp01_usart2.h"
#include "LOOP_BUFF.h"
#include "mqtt_interface.h"
#include "MQTT.h"
extern BufferTypeDef  ESP_RX_BUF;  //���λ��������

extern double distance;
extern uint8_t dth11_recv[5];
/******************************************************
    @����������
        ���������ڴ��������д�������������ں˶���
*******************************************************/
void My_First_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        //�����ٽ���
        taskENTER_CRITICAL();
        
        /********************* �ں˶���Ĵ��� ******************************/
        /*  */
        
        /********************* ����Ĵ��� **************************/
        xReturn = xTaskCreate(
                (TaskFunction_t) DHT11_Fun, //������
                (const char *) "DHT11_Fun", //��������
                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
                (void * const) NULL, //�������
                (UBaseType_t) 3, //���ȼ�
                (TaskHandle_t * const) &DHT11_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---DHT11_Task create ok---\n");
        }
        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) SRF05_Fun, //������
//                (const char *) "SRF05_Fun", //��������
//                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
//                (void * const) NULL, //�������
//                (UBaseType_t) 3, //���ȼ�
//                (TaskHandle_t * const) &SRF05_Handle );
//   
//        if(xReturn != pdFALSE)
//        {
//            printf("---SRF05_Task create ok---\n");
//        }
        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) ESP_SEND_Fun, //������
//                (const char *) "ESP_SEND_Fun", //��������
//                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
//                (void * const) NULL, //�������
//                (UBaseType_t) 5, //���ȼ�
//                (TaskHandle_t * const) &ESP_SEND_Handle );
//        if(xReturn != pdFALSE)
//        {
//            printf("---ESP_SEND Task create ok---\n");
//        }
//        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) ESP_RECV_Fun, //������
//                (const char *) "ESP_RECV_Fun", //��������
//                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
//                (void * const) NULL, //�������
//                (UBaseType_t) 5, //���ȼ�
//                (TaskHandle_t * const) &ESP_RECV_Handle );
//        if(xReturn != pdFALSE)
//        {
//            printf("---ESP_RECV Task create ok---\n");
//        }
        //�˳��ٽ���
        taskEXIT_CRITICAL();
        
        //ɾ��������
        vTaskDelete(First_TaskHandle);
    }
}

/*----------------------------------------------
            **** ���������� ***
------------------------------------------------*/
/*********************************
    ����������DHT11�����¶�
**********************************/
void DHT11_Fun(void *arg)  
{
    while(1)
    {
        DHT11_Start();
        DHT11_Recv_Data();
        DHT11_END();
		
        vTaskDelay(500);
		
		char payload[20];
        snprintf(payload, sizeof(payload), "%d:%d", dth11_recv[2],dth11_recv[3]);
        MTTPublishStruct_t s1 = {0};
        s1.DUP = 0;
        s1.Qos = 0;
        s1.RETAN = 0;
        s1.topic = "stm32/dth11";
        s1.payload = payload;
        MQTTPublish(&s1);
	
    }
}

/*********************************
    ����������SRF05���
**********************************/
void SRF05_Fun(void *arg)  //����
{
    while(1)
    { 
        Srf05_Start();
        vTaskDelay(200);
		User_Subscribe();
		char payload[20];
        snprintf(payload, sizeof(payload), "%.2f", distance);
        MTTPublishStruct_t s1 = {0};
        s1.DUP = 0;
        s1.Qos = 0;
        s1.RETAN = 0;
        s1.topic = "stm32/distance";
        s1.payload = payload;
        MQTTPublish(&s1);
    }
}
/*********************************
    ����������ESP �ķ��ͺ���
**********************************/
void ESP_SEND_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        printf("---Task ESP_SEND_FUN---\n");
        //Usart2_SendString("this is ESP_SEND Task");
		User_Publish();
        vTaskDelay(500);
    }
}
/*********************************
    ����������ESP �Ľ��պ���
**********************************/
void ESP_RECV_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        printf("---Task ESP_RECV_FUN---\n");
        Buffer_Print(&ESP_RX_BUF);
        vTaskDelay(500);
    }
}


//��ʱ���Ļص�����
void Tim_CallBack(void *arg)
{
    printf("This is Timer CallBack\n");
}
//����������
void vApplicationIdleHook(void)
{
    printf("This is idle task\n");
    
}


















