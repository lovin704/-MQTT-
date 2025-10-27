#ifndef __MY_FREERTOS_FUN_H__
#define __MY_FREERTOS_FUN_H_
#include "stm32f4xx.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
/*----------------------------------------------
    **** �ں��������:������ź����������� ***
------------------------------------------------*/

TaskHandle_t First_TaskHandle = NULL;  //��ʼ������

TaskHandle_t DHT11_Handle = NULL;  //DHT11��������
TaskHandle_t SRF05_Handle = NULL;  //SRF05��������
TaskHandle_t ESP_SEND_Handle = NULL;  //ESP01�ķ���������
TaskHandle_t ESP_RECV_Handle = NULL;  //ESP01�Ľ���������

TimerHandle_t Time_Handle = NULL;      //�����ʱ�����
SemaphoreHandle_t Semp_Handle = NULL;  //�ź����ľ��
QueueHandle_t Test_Queue = NULL;  //���о��


/*----------------------------------------------
            **** ������������ ***
------------------------------------------------*/
void My_First_Fun(void *arg);  /* ��һ�������� */

void DHT11_Fun(void *arg);  //DHT11��������
void SRF05_Fun(void *arg);  //SRF05���������
void ESP_RECV_Fun(void *arg);
void ESP_SEND_Fun(void *arg);

void Tim_CallBack(void *arg);//��ʱ���Ļص�����
#endif




