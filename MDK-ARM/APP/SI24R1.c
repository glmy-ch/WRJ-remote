#include "SI24R1.h"

uint8_t  TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // 定义一个静态发送地址
// 使用HAL库进行片选信号的拉低拉高
#define CSN_LOW()	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET)
#define CSN_HIGH()	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET)
// 使用HAL库进行CE信号的拉低拉高
#define CE_LOW()		HAL_GPIO_WritePin(SI_EN_GPIO_Port,SI_EN_Pin,GPIO_PIN_RESET)
#define CE_HIGH()		HAL_GPIO_WritePin(SI_EN_GPIO_Port,SI_EN_Pin,GPIO_PIN_SET)
static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t bit_ctr;
	// 使用HAL库进行1个数据的接发
	// 1.使用SPI接口的句柄 2.需要发送的数据 3.接收数据的缓存区 4.一次传输的数据大小 5.等待的时间
	HAL_SPI_TransmitReceive(&hspi1, &byte, &bit_ctr, 1, 1000);
	return(bit_ctr);                              
}
/********************************************************
函数功能：SI24R1引脚初始化                
入口参数：无
返回  值：无
*********************************************************/
// void SI24R1_Init(void)
// {
// 	SCK = 0; 													//SPI时钟线拉低
// 	CSN = 1;				
// 	CE 	= 0;				
// 	IRQ = 1;
// }
/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
 
	CSN_LOW();              //将片选引脚拉低开始进行通讯         
	status = SPI_RW(reg);	// 发送寄存器地址	 		
	SPI_RW(value);	  		//发送要写入的数据
	CSN_HIGH();  	        //将片选引脚拉高结束通讯
	return(status);
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  CSN_LOW();               //将片选引脚拉低开始进行通讯                         			
  status = SPI_RW(reg);    //发生需要写入的地址                  
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)  //循环的发送任务    
    SPI_RW(*pBuf++);
  CSN_HIGH();  	           //将片选引脚拉高结束通讯                                   	
  return(status);       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t SI24R1_Read_Reg(uint8_t reg)
{
 	uint8_t value;

	CSN_LOW();                 //将片选引脚拉低开始进行通讯      
	SPI_RW(reg);			
	value = SPI_RW(0);
	CSN_HIGH();  	           //将片选引脚拉高结束通讯               

	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
	uint8_t status,byte_ctr;

  CSN_LOW();                 //将片选引脚拉低开始进行通讯                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //读取数据，低字节在前
  CSN_HIGH();                                    

  return(status);    
}


/********************************************************
函数功能：SI24R1接收模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE_LOW();       //将CE拉低 进入到CMD模式
	SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);               						// 使能接收通道0自动应答
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);           						// 使能接收通道0
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);                 						// 选择射频通道0x40
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  						// 接收通道0选择和发送通道相同有效数据宽度
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x06);            						// 数据传输率1Mbps，发射功率4dBm
	SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0f);              						// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, 0xff);  												//清除所有的中断标志位
	CE_HIGH();      //将CE拉高 退出CMD模式                                  									// 拉高CE启动接收设备
}						


/********************************************************
函数功能：SI24R1发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE_LOW();     //将CE拉低 进入到CMD模式   
	SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);       											// 使能接收通道0自动应答
	SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);   											// 使能接收通道0
	SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x0a);  											// 自动重发延时等待250us+86us，自动重发10次
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, 40);         											// 选择射频通道0x40
	SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x06);    											// 数据传输率1Mbps，发射功率4dBm
	SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0e);      											// CRC使能，16位CRC校验，上电
	CE_HIGH();   //将CE拉高 退出CMD模式 
}


/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
uint8_t SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	state = SI24R1_Read_Reg(STATUS);  			                 //读取状态寄存器的值    	  
	SI24R1_Write_Reg(SI24R1_WRITE_REG+STATUS,state);               //清除RX_DS中断标志

	if(state & RX_DR)								                           //接收到数据
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //读取数据
		SI24R1_Write_Reg(FLUSH_RX,0xff);					              //清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;                                                   //没收到任何数据
}


/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返回  值： 1:达到最大重发次数，发送失败 
          0:发送成功            
          1:发送失败                  
*********************************************************/
uint8_t SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
	CE_LOW();		                                        //CE拉低，使能SI24R1配置
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //写数据到TX FIFO,32个字节
 	CE_HIGH();										        //CE置高，使能发送	   
	
	//while(IRQ == 1);										//等待发送完成
	state = SI24R1_Read_Reg(STATUS);  //读取状态寄存器的值
	while( ((state & TX_DS) == 0) && ((state & MAX_RT)==0) )   //检查10次重发数据据 
	{
		state = SI24R1_Read_Reg(STATUS);
		vTaskDelay(10);
	}											             //读取状态寄存器的值	   
	SI24R1_Write_Reg(SI24R1_WRITE_REG+STATUS, state); 		 //清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)										 //达到最大重发次数
	{
		SI24R1_Write_Reg(FLUSH_TX,0xff);					 //清除TX FIFO寄存器 
		return 1; 
	}
	if(state&TX_DS)											 //发送完成
	{
		return 0;
	}
	return 1;		//发送失败																				  //发送失败
}

uint8_t si24r1_rx_buff[5];
// 测试SI24R1通讯
uint8_t SI24R1_check(void)
{
   // 读写SPI时先进行延时操作，等待SI24R1上电完成
   HAL_Delay(200);
   // 1.测试SPI通讯是否可以正常读写寄存器
   // 2.SI24R1需要先读一遍，保证SPI正常后再写入
   SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR,si24r1_rx_buff,TX_ADR_WIDTH);
   // 1.1 写入发送的地址
   SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR,TX_ADDRESS, TX_ADR_WIDTH); 
   // 1.2 读取同样的数据
   SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR,si24r1_rx_buff,TX_ADR_WIDTH);	
   if(si24r1_rx_buff[0] == TX_ADDRESS[0])
   {
      return 0; // SPI通信正常
   }
   return 1; // SPI通信异常	
}
void SI24R1_Init(void)
{
	// 上电后进行200MS的延时处理
	HAL_Delay(200);
	// 初始化之前先进行SPI通信校验
    while(SI24R1_check()) ;
	{
		HAL_Delay(10);
	}
	// 进行SI24R1接收初始化
	SI24R1_RX_Mode();
	printf("SI24R1_Init success\n");
}

