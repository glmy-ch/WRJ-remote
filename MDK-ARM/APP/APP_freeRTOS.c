#include "APP_freeRTOS.h"
#include "com_debug.h"
#include "tp4336.h"
#include "SI24R1.h"
#include "key.h"
#include "joystick.h"
#include "APP_process_data.h"
#include "APP_transmit.h"
/*==================================电源控制任务参数===========================*/
#define task_power_key_SIZE 128    
#define task_power_key_PRIORITY 4
TaskHandle_t power_key_handle;
#define task_power_key_PERIOD 10000  //每10s执行一次
/*==========================SI24R1(2.4Hz通讯任务参数)=========================*/
#define task_SI24R1_key_SIZE 128
#define task_SI24R1_key_PRIORITY 3
TaskHandle_t SI24R1_handle;
#define task_SI24R1_key_PERIOD 6     //每6ms执行一次
/*==========================按键任务参数=====================================*/
#define task_key_SIZE 128
#define task_key_PRIORITY 2
TaskHandle_t task_key_handle;
#define task_key_PERIOD 20     //每20ms执行一次
/*==========================遥感数据采集参数=================================*/
#define task_joystick_SIZE 128
#define task_joystick_PRIORITY 2
TaskHandle_t task_joystick_handle;
#define task_joystick_PERIOD 20  //每20ms执行一次
/****************************任务函数的构建*********************************/
void task_power_key (void * args);
void task_SI24R1 (void * args);
void task_key (void * args);
void task_joystick (void * args);
/****************************遥感数据结构体**********************************/

/**
 * @brief 任务的创建和启动
 */
void StartTask(void)
{
// 1.创建一个电源控制的任务
      //1.任务回调函数 2.任务名称 3.任务所占空间大小 4.传入任务的参数 5.任务的优先级 6.任务的句柄
    xTaskCreate(task_power_key, "task1", task_power_key_SIZE, NULL, task_power_key_PRIORITY,
        &power_key_handle);
// 2.创建一个SI24R1发送的任务
    //1.任务回调函数 2.任务名称 3.任务所占空间大小 4.传入任务的参数 5.任务的优先级 6.任务的句柄
    xTaskCreate(task_SI24R1, "task2", task_SI24R1_key_SIZE, NULL, task_SI24R1_key_PRIORITY,
        &SI24R1_handle);
// 3.创建按键任务
    xTaskCreate(task_key, "task3", task_key_SIZE, NULL, task_key_PRIORITY,
         &task_key_handle);
// 4.创建遥感数据采集任务
    xTaskCreate(task_joystick, "task4", task_joystick_SIZE, NULL, task_joystick_PRIORITY,
         &task_joystick_handle);
    //启动任务调度
    vTaskStartScheduler();
}
/**
 * @brief 电源控制任务函数 (每10S向电池写0 防止电池管理处于休眠模式)
 */
 void  task_power_key( void * args)
{    //获取当前时间
     uint32_t xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, task_power_key_PERIOD); //每过10s执行一次
        void tp4336_power_key(void);
    }
}
/**
 * @brief SI24R1(2.4Hz通讯任务函数)
 */
// 申明一个用于存储发送数据的缓冲区
uint8_t SI24R1_TX_BUF[TX_PLOAD_WIDTH] = {0};
void task_SI24R1(void * args)
{
    uint32_t xLastWakeTime = xTaskGetTickCount();         //获取当前时间
    while(1)
    {
        APP_transmit_Data();          
        vTaskDelayUntil(&xLastWakeTime, task_SI24R1_key_PERIOD); //每过6ms执行一次
    }
}
/**
 * 
 * @brief 按键任务函数
 */
void task_key(void * args)
{
    uint32_t xLastWakeTime = xTaskGetTickCount();         //获取当前时间
    while(1)
    {
         process_key();
         vTaskDelayUntil(&xLastWakeTime, task_joystick_PERIOD); //每过20ms执行一次
    }
}
/**
 * @brief 遥感数据采集任务函数
 */
void task_joystick(void * args)
{
    uint32_t xLastWakeTime = xTaskGetTickCount();         //获取当前时间
    // 初始化遥感数据采集
    joystick_init();
    while(1)
    {
        joystick_get_value(&joystick);
        vTaskDelayUntil(&xLastWakeTime, task_joystick_PERIOD); //每过20ms执行一次
    }
}
