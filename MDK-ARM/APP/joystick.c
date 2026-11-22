#include "joystick.h"
/**
 * @brief 初始化摇杆
 */
uint16_t adc_value[4];
void joystick_init(void)
{
    //使用ADC DMA模式采集摇杆的数据
    // 启动ADC转换
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 2);
}
/**
 * @brief 获取摇杆的数据
 */
void joystick_get_value(joystick_t *joystick)
{
    //DMA不需要经过CPU的计算，所以直接读取数据
    joystick->thr = adc_value[0];
    joystick->yaw = adc_value[1];
    joystick->pit = adc_value[2];
    joystick->rol = adc_value[3];
}