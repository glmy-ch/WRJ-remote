#ifndef JOYSTICK_H
#define JOYSTICK_H
#include "adc.h"
// 定义一个结构体，用于存储摇杆的状态
typedef struct {
    int16_t thr;
    int16_t yaw;
    int16_t pit;
    int16_t rol;
} joystick_t;
/**
 * @brief 初始化摇杆
 */
void joystick_init(void);
/**
 * @brief 读取摇杆的值
 */
void joystick_get_value(joystick_t *joystick);

#endif // JOYSTICK_H
