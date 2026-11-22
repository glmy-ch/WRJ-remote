#include "tp4336.h"

void tp4336_power_key(void)
{   
    //向power_key写低电平 
    HAL_GPIO_WritePin(power_key_GPIO_Port, power_key_Pin, GPIO_PIN_RESET);
    //使用freeRTOS的延时函数延时100MS
    vTaskDelay(100);
    //将POWER_KEY写高电平
    HAL_GPIO_WritePin(power_key_GPIO_Port, power_key_Pin, GPIO_PIN_SET);
}

