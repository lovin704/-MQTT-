#include "stm32f4xx.h"                  // Device header
#include "my_freertos_fun.h"
#include "stdio.h"
#include "my_dht11.h"
#include "my_srf05.h"
#include "esp01_usart2.h"
#include "LOOP_BUFF.h"
#include "mqtt_interface.h"
#include "MQTT.h"
extern BufferTypeDef  ESP_RX_BUF;  //环形缓冲区句柄

extern double distance;
extern uint8_t dth11_recv[5];
/******************************************************
    @任务描述：
        启动任务，在此任务数中创建其他任务和内核对象
*******************************************************/
void My_First_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        //进入临界区
        taskENTER_CRITICAL();
        
        /********************* 内核对象的创建 ******************************/
        /*  */
        
        /********************* 任务的创建 **************************/
        xReturn = xTaskCreate(
                (TaskFunction_t) DHT11_Fun, //任务函数
                (const char *) "DHT11_Fun", //任务名称
                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
                (void * const) NULL, //任务参数
                (UBaseType_t) 3, //优先级
                (TaskHandle_t * const) &DHT11_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---DHT11_Task create ok---\n");
        }
        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) SRF05_Fun, //任务函数
//                (const char *) "SRF05_Fun", //任务名称
//                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
//                (void * const) NULL, //任务参数
//                (UBaseType_t) 3, //优先级
//                (TaskHandle_t * const) &SRF05_Handle );
//   
//        if(xReturn != pdFALSE)
//        {
//            printf("---SRF05_Task create ok---\n");
//        }
        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) ESP_SEND_Fun, //任务函数
//                (const char *) "ESP_SEND_Fun", //任务名称
//                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
//                (void * const) NULL, //任务参数
//                (UBaseType_t) 5, //优先级
//                (TaskHandle_t * const) &ESP_SEND_Handle );
//        if(xReturn != pdFALSE)
//        {
//            printf("---ESP_SEND Task create ok---\n");
//        }
//        /* -----------------------------------------*/
//        xReturn = xTaskCreate(
//                (TaskFunction_t) ESP_RECV_Fun, //任务函数
//                (const char *) "ESP_RECV_Fun", //任务名称
//                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
//                (void * const) NULL, //任务参数
//                (UBaseType_t) 5, //优先级
//                (TaskHandle_t * const) &ESP_RECV_Handle );
//        if(xReturn != pdFALSE)
//        {
//            printf("---ESP_RECV Task create ok---\n");
//        }
        //退出临界区
        taskEXIT_CRITICAL();
        
        //删除主任务
        vTaskDelete(First_TaskHandle);
    }
}

/*----------------------------------------------
            **** 任务函数定义 ***
------------------------------------------------*/
/*********************************
    任务描述：DHT11测量温度
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
    任务描述：SRF05测距
**********************************/
void SRF05_Fun(void *arg)  //发送
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
    任务描述：ESP 的发送函数
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
    任务描述：ESP 的接收函数
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


//定时器的回调函数
void Tim_CallBack(void *arg)
{
    printf("This is Timer CallBack\n");
}
//空闲任务函数
void vApplicationIdleHook(void)
{
    printf("This is idle task\n");
    
}


















