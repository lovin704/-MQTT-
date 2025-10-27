#ifndef __MY_FREERTOS_FUN_H__
#define __MY_FREERTOS_FUN_H_
#include "stm32f4xx.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
/*----------------------------------------------
    **** 内核组件定义:句柄、信号量、互斥锁 ***
------------------------------------------------*/

TaskHandle_t First_TaskHandle = NULL;  //开始任务句柄

TaskHandle_t DHT11_Handle = NULL;  //DHT11的任务句柄
TaskHandle_t SRF05_Handle = NULL;  //SRF05的任务句柄
TaskHandle_t ESP_SEND_Handle = NULL;  //ESP01的发送任务句柄
TaskHandle_t ESP_RECV_Handle = NULL;  //ESP01的接收任务句柄

TimerHandle_t Time_Handle = NULL;      //软件定时器句柄
SemaphoreHandle_t Semp_Handle = NULL;  //信号量的句柄
QueueHandle_t Test_Queue = NULL;  //队列句柄


/*----------------------------------------------
            **** 任务函数的声明 ***
------------------------------------------------*/
void My_First_Fun(void *arg);  /* 第一个任务函数 */

void DHT11_Fun(void *arg);  //DHT11的任务函数
void SRF05_Fun(void *arg);  //SRF05测距任务函数
void ESP_RECV_Fun(void *arg);
void ESP_SEND_Fun(void *arg);

void Tim_CallBack(void *arg);//定时器的回调函数
#endif




