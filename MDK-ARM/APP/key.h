#ifndef KEY_H
#define KEY_H
#include "freeRTOS.h"
#include "task.h"
#include "gpio.h"

/**
 * @brief 创建一个按键类型枚举
 */
typedef enum
{
    KEY_NONE,    
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_RIGHT_X_LONG,
}KEY_T;

/**
 * @brief 检查按键是否按下
 */
KEY_T key_init(void);

#endif
