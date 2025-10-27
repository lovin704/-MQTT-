#include "stm32f4xx.h"                  // Device header
#include "my_freertos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* --------------------------
        系统测试函数          
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
        test_fun,  //任务函数
        "test_fun", //任务名称
        128, //任务栈的大小
        NULL,//传给任务的参数
        1, //任务优先级
        &Test_Handle); //任务句柄
    
    vTaskStartScheduler();
    
}

/*----------------------------------------------
            **** 任务创建 ***
------------------------------------------------*/
/* 内核组件定义： 句柄、信号量、互斥锁、...*/
TaskHandle_t First_TaskHandle = NULL;  //开始任务句柄

TaskHandle_t Task1_Handle = NULL;
TaskHandle_t Task2_Handle = NULL;
TaskHandle_t Task3_Handle = NULL;

TimerHandle_t Time_Handle = NULL;      //软件定时器句柄
SemaphoreHandle_t Semp_Handle = NULL;  //信号量的句柄
QueueHandle_t Test_Queue = NULL;  //队列句柄


/*----------------------------------------------
            **** 任务函数定义 ***
------------------------------------------------*/
/*********************************
    任务描述：
**********************************/
void Task1_Fun(void *arg)  //接收
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
            //释放信号量
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
    任务描述：
**********************************/
void Task2_Fun(void *arg)  //发送
{
    BaseType_t xReturn;
    while(1)
    { 
        vTaskDelay(100); 
        for(uint8_t i = 0 ;i < 10; i++)
        {
            printf("---This is Task2---\n");
        }
        //删除任务2
        vTaskDelete(Task2_Handle);
    }
}
/*********************************
    任务描述：
**********************************/
void Task3_Fun(void *arg)
{
    BaseType_t xReturn;
    while(1)
    {
        vTaskDelay(200);  //任务3先阻塞住，去运行任务1 和任务2
        printf("Task3 get semphore\n");
        xReturn = xSemaphoreTake(Semp_Handle,portMAX_DELAY);
        if(xReturn == pdTRUE)
        {
            printf("--------This is Task3 -------\n");
        }
        vTaskDelete(Task3_Handle);
    }
}

//定时器的回调函数
void Tim_CallBack(void *arg)
{
    printf("This is Timer CallBack\n");
}

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
                (TaskFunction_t) Task1_Fun, //任务函数
                (const char *) "Task1_Fun", //任务名称
                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
                (void * const) NULL, //任务参数
                (UBaseType_t) 2, //优先级
                (TaskHandle_t * const) &Task1_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---Task1 create ok---\n");
        }
        /* -----------------------------------------*/
        xReturn = xTaskCreate(
                (TaskFunction_t) Task2_Fun, //任务函数
                (const char *) "Task2_Fun", //任务名称
                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
                (void * const) NULL, //任务参数
                (UBaseType_t) 3, //优先级
                (TaskHandle_t * const) &Task2_Handle );
   
        if(xReturn != pdFALSE)
        {
            printf("---Task2 create ok---\n");
        }
        /* -----------------------------------------*/
        xReturn = xTaskCreate(
                (TaskFunction_t) Task3_Fun, //任务函数
                (const char *) "Task3_Fun", //任务名称
                (const configSTACK_DEPTH_TYPE) 128, //栈的大小
                (void * const) NULL, //任务参数
                (UBaseType_t) 5, //优先级
                (TaskHandle_t * const) &Task3_Handle );
        if(xReturn != pdFALSE)
        {
            printf("---Task3 create ok---\n");
        }
        //退出临界区
        taskEXIT_CRITICAL();
        
        //删除主任务
        vTaskDelete(First_TaskHandle);
    }
}




//空闲任务函数
void vApplicationIdleHook(void)
{
    printf("This is idle task\n");
    
}


















