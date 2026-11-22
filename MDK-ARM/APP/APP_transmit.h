#ifndef APP_TRANSMIT_H
#define APP_TRANSMIT_H
#include "SI24R1.h"
#include "APP_process_data.h"
#include "FreeRTOS.h"
#include "task.h"
/**
 * @brief 将打包好的数据通过2.4GHz通讯发送到无人机中
 */
void APP_transmit_Data(void);


#endif

