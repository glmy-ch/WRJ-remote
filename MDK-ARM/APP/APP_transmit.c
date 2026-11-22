#include "APP_transmit.h"
 extern Remote_Data_t remote;
 #define FRAME_HEAD_CHECK_1   'L'
 #define FRAME_HEAD_CHECK_2   'Q'
 #define FRAME_HEAD_CHECK_3   'P'
uint8_t com_buff[TX_PLOAD_WIDTH] ={0};
void APP_transmit_Data(void)
{
    uint32_t sum=0;
    // 1.进入到发送模式
    SI24R1_TX_Mode();
    // 进入临界区 防止将状体置0的时候跳入别的任务
    taskENTER_CRITICAL();
    
    // 2.打包数据并经行发送 (数据本身10个字节 帧头校验3字节 校验位4字节 一共17字节)
    // 包装校验码
    com_buff[0] = (uint8_t) FRAME_HEAD_CHECK_1;
    com_buff[1] = (uint8_t) FRAME_HEAD_CHECK_2;
    com_buff[2] = (uint8_t) FRAME_HEAD_CHECK_3;
    // 包装数据 高八位在前
    com_buff[3] = (remote.thr>>8)& 0xff;
    com_buff[4] =  remote.thr & 0xff;

    com_buff[5] = (remote.yaw>>8)& 0xff;
    com_buff[6] =  remote.yaw & 0xff;

    com_buff[7] = (remote.pit>>8)& 0xff;
    com_buff[8] = remote.pit & 0xff;

    com_buff[9] = (remote.rol>>8)& 0xff;
    com_buff[10] = remote.rol & 0xff;

  
    com_buff[11] = (remote.shutodwn>>8)& 0xff;
    remote.shutodwn =0;
    com_buff[12] = remote.fix_height & 0xff;
    remote.fix_height =0;
    // 退出临界区
    taskEXIT_CRITICAL();
    //数据校验位包装 高位在前
    for(uint8_t i = 0; i < 13; i++)
    {
        sum += com_buff[i] ;
    }
    com_buff[13] = (sum>>24) & 0xff;
    com_buff[14] = (sum>>16) & 0xff;
    com_buff[15] = (sum>>8) & 0xff;
    com_buff[16] = sum & 0xff;
    // 使用函数发数据
    SI24R1_TxPacket(com_buff);
    // 4.恢复到接收模式
    SI24R1_RX_Mode();

}

