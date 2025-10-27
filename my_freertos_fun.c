#include "stm32f4xx.h"                  // Device header
#include "my_freertos_fun.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"



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
