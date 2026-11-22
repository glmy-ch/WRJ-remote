#include "APP_process_data.h"
/*创建一个全局变量来保存摇杆的数据*/
 joystick_t joystick={0};
 Remote_Data_t remote ={0};
/*区分遥感的控制值和按键的控制值*/
int16_t key_pit_offset =0;
int16_t key_rol_offset =0;
/*遥感的偏移值*/
int pit_offset =0;
int rol_offset =0;
int thr_offset =0;
int yaw_offset =0;
/**
 * @brief 遥感数据校准
 */
void joystick_calibrate(void)
{
    // 在进行遥感校准的时候将按键的偏移值清零
    key_pit_offset =0;
    key_rol_offset =0;
    // 0.创建记录整体偏移量的值
    int pit_Sum =0;
    int rol_Sum =0; 
    int thr_Sum =0; 
    int yaw_Sum =0;
    // 1.获取摇杆的数据
    joystick_get_value(&joystick);
    // 2.采集10次偏移的数据
    for(uint8_t i = 0; i < 10; i++)
    { 
        pit_Sum += joystick.pit;
        rol_Sum += joystick.rol;
        thr_Sum += joystick.thr;
        yaw_Sum += joystick.yaw;
    }
    // 3.计算平局偏移值
    pit_offset += pit_Sum/10;
    rol_offset += rol_Sum/10;
    thr_offset += thr_Sum/10;
    yaw_offset += yaw_Sum/10;
}
/**
 * @brief 处理按键的数据
 */
void process_key(void)
{
    // 1.获取按键的数据
    KEY_T key = key_init();
    // 2.判断获取的按键值，进行相关数据的处理
    if(key == KEY_UP )
    {
        //如果按下上面的按键偏移值+10
        key_pit_offset+=10;
    }
    else if (key==KEY_DOWN)
    {
        //如果向下按键按下偏移值-10
        key_pit_offset-=10;
    }
    else if (key==KEY_LEFT)
    {
        //如果向左按键按下偏移值-10
        key_rol_offset-=10;
    }
    else if (key==KEY_RIGHT)
    {
        //如果向右按键按下偏移值+10
        key_rol_offset+=10;
    }
    else if(key == KEY_LEFT_X )
    {
        //按下左边的值 将关机键置1
        remote.shutodwn =1;
    }
    else if(key == KEY_RIGHT_X)
    {
        //短按下右边的值 进入悬停模式
        remote.fix_height =1;
    }
    else if(key == KEY_RIGHT_X_LONG)
    {
        //长按下右边的值进入到校准遥感
        joystick_calibrate();
    }
}

/**
 * @brief 处理遥感的数据
 */
void joystick_process(void)
{
    // 0.加入临界区，防止多任务同时访问数据
    taskENTER_CRITICAL();
    // 1.获取遥感ADC采集的数据
    joystick_get_value(&joystick);
    // 2.将获取到的值进行转换  等比例换算成1000以内的值
    joystick.pit = 1000-((uint32_t)joystick.pit*1000/4095); 
    joystick.rol = 1000-((uint32_t)joystick.rol*1000/4095);
    joystick.thr = 1000-((uint32_t)joystick.thr*1000/4095);
    joystick.yaw = 1000-((uint32_t)joystick.yaw*1000/4095);
    // 3.减去校验所得的偏移值
    joystick.pit -= pit_offset;
    joystick.rol -= rol_offset;
    joystick.thr -= thr_offset;
    joystick.yaw -= yaw_offset;
    // 4.加上按键的偏移值
    joystick.pit += key_pit_offset;
    joystick.rol += key_rol_offset;
    // 5.限制摇杆的值在-1000~1000之间
    joystick.pit = COM_Tool(&joystick.pit, 0, 1000);
    joystick.rol = COM_Tool(&joystick.rol, 0, 1000);
    joystick.thr = COM_Tool(&joystick.thr, 0, 1000);
    joystick.yaw = COM_Tool(&joystick.yaw, 0, 1000);
    // 6.退出临界区
    taskEXIT_CRITICAL();
    // 7.将获取到的数据幅值给变量
    remote.pit = joystick.pit;
    remote.rol = joystick.rol;
    remote.thr = joystick.thr;
    remote.yaw = joystick.yaw;
    // 8.将获取到的数据打印出来
    // printf(":%d,%d,%d,%d",joystick.pit, joystick.rol, joystick.thr, joystick.yaw);
}

