#include "stm32f4xx.h"                  // Device header
#include "my_freertos_fun.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"



/*----------------------------------------------
            **** ���������� ***
------------------------------------------------*/
/*********************************
    ����������
**********************************/
void Task1_Fun(void *arg)  //����
{
    BaseType_t xReturn;
    uint32_t buffer = 5;
    while(1)
    {
        //xReturn = xQueueReceive(Test_Queue,&buffer,0);
        xReturn = xSemaphoreTake(Semp_Handle,portMAX_DELAY);
        if(pdTRUE == xReturn)
        {
            for(uint8_t i = 0; i < 100; i++)
            {
                printf("---This is Task1---\n");
            }
            //�ͷ��ź���
            xReturn = xSemaphoreGive(Semp_Handle);
            if(xReturn == pdTRUE)
            {
                printf("Task1 GIVE OK\n");
            }
        }
        else
        {
            printf("RECEIVE data is ERROR\n");
        }
        vTaskDelay(100);
    }
}

/*********************************
    ����������
**********************************/
void Task2_Fun(void *arg)  //����
{
    BaseType_t xReturn;
    while(1)
    { 
        vTaskDelay(100); 
        for(uint8_t i = 0 ;i < 10; i++)
        {
            printf("---This is Task2---\n");
        }
        //ɾ������2
        vTaskDelete(Task2_Handle);
    }
}
/*********************************
    ����������
**********************************/
void Task3_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        vTaskDelay(200);  //����3������ס��ȥ��������1 ������2
        printf("Task3 get semphore\n");
        xReturn = xSemaphoreTake(Semp_Handle,portMAX_DELAY);
        if(xReturn == pdTRUE)
        {
            printf("--------This is Task3 -------\n");
        }
        vTaskDelete(Task3_Handle);
    }
}

//��ʱ���Ļص�����
void Tim_CallBack(void *arg)
{
    printf("This is Timer CallBack\n");
}
