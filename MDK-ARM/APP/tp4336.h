#ifndef TP4336_H
#define TP4336_H
#include "freeRTOS.h"
#include "task.h"
#include "main.h"
// 每10s钟向power_key 写低电平
void tp4336_power_key(void);
#endif 
