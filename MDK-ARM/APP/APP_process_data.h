#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H
#include "key.h"
#include "joystick.h"
#include "Com_tool.h"
#include "stdio.h"
/**
 * @brief 声明一个控制数据结构体
 */
typedef struct
{
    int thr;
    int yaw;
    int pit;
    int rol;
    uint8_t shutodwn;
    uint8_t fix_height;
} Remote_Data_t;
/*创建全局变量*/
extern joystick_t joystick;
extern Remote_Data_t remote;
extern int16_t key_pit_offset;
extern int16_t key_rol_offset;
extern int pit_offset;
extern int rol_offset;
extern int thr_offset;
/**
 * @brirf 处理按键数据
 */
void process_key(void);

/**
 * @brief 处理遥感数据
 */
void joystick_process(void);

#endif

