#include "key.h"
/**
 * 
 * @brief 检查按键是否被按下
 * 
 */

KEY_T key_init(void)
{
    if(HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET);
        return KEY_LEFT_X;     //按键松开返回数据
    }

    else if(HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {   
        TickType_t start_time = xTaskGetTickCount(); //记入当前时间如果按键按下的时间超过1S视为长按
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET);
        if(xTaskGetTickCount() - start_time > 1000)
        { //如果按键按下的时间超过1S视为长按
            return KEY_RIGHT_X_LONG;
        }
        else
        { //如果按键按下的时间不超过1S视为短按
            return KEY_RIGHT_X;
        }
        
    }
    else if(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET);
            return KEY_UP;
    }
    else if(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET);
            return KEY_DOWN;
    }
    else if(HAL_GPIO_ReadPin(KET_LEFT_GPIO_Port, KET_LEFT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KET_LEFT_GPIO_Port, KET_LEFT_Pin) == GPIO_PIN_RESET);
            return KEY_LEFT;
    }
    else if(HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(5); //延时5ms做消抖处理
        /* 如果按键没有被释放，则继续读取按键状态 */
        while(HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET);
            return KEY_RIGHT;
        
    }
    /* 如果没有按下任何按键，则返回KEY_NONE */
    return KEY_NONE;    
}
