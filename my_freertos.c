#include "stm32f4xx.h"                  // Device header
#include "my_freertos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* --------------------------
        ϵͳ���Ժ���          
------------------------------*/
TaskHandle_t Test_Handle;
void test_fun(void)
{
    while(1)
    {
        printf("This is test fun\n");
    }
}

void FRTOS_Test(void)
{
    xTaskCreate(
        test_fun,  //������
        "test_fun", //��������
        128, //����ջ�Ĵ�С
        NULL,//��������Ĳ���
        1, //�������ȼ�
        &Test_Handle); //������
    
    vTaskStartScheduler();
    
}

/*----------------------------------------------
            **** ���񴴽� ***
------------------------------------------------*/
/* �ں�������壺 ������ź�������������...*/
TaskHandle_t First_TaskHandle = NULL;  //��ʼ������

TaskHandle_t Task1_Handle = NULL;
TaskHandle_t Task2_Handle = NULL;
TaskHandle_t Task3_Handle = NULL;

TimerHandle_t Time_Handle = NULL;      //�����ʱ�����
SemaphoreHandle_t Semp_Handle = NULL;  //�ź����ľ��
QueueHandle_t Test_Queue = NULL;  //���о��


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
                (TaskFunction_t) Task1_Fun, //������
                (const char *) "Task1_Fun", //��������
                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
                (void * const) NULL, //�������
                (UBaseType_t) 2, //���ȼ�
                (TaskHandle_t * const) &Task1_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---Task1 create ok---\n");
        }
        /* -----------------------------------------*/
        xReturn = xTaskCreate(
                (TaskFunction_t) Task2_Fun, //������
                (const char *) "Task2_Fun", //��������
                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
                (void * const) NULL, //�������
                (UBaseType_t) 3, //���ȼ�
                (TaskHandle_t * const) &Task2_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---Task2 create ok---\n");
        }
        /* -----------------------------------------*/
        xReturn = xTaskCreate(
                (TaskFunction_t) Task3_Fun, //������
                (const char *) "Task3_Fun", //��������
                (const configSTACK_DEPTH_TYPE) 128, //ջ�Ĵ�С
                (void * const) NULL, //�������
                (UBaseType_t) 5, //���ȼ�
                (TaskHandle_t * const) &Task3_Handle );
        if(xReturn != pdFALSE)
        {
            printf("---Task3 create ok---\n");
        }
        //�˳��ٽ���
        taskEXIT_CRITICAL();
        
        //ɾ��������
        vTaskDelete(First_TaskHandle);
    }
}




//����������
void vApplicationIdleHook(void)
{
    printf("This is idle task\n");
    
}


















