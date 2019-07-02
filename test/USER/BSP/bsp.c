/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*
* 本例程在 奋斗版STM32开发板MINI，V2,2.1,V3上调试通过           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: bsp.c
* 内容简述:
*       本例程提供了硬件平台的初始化
		
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.2    2011-07-04 sun68  创建该文件
*
*/
#include "includes.h"
//#include "demo.h"


#define ADC1_DR_Address    ((u32)0x4001244C)  	        //ADC1外设地址
/* 定义了触摸芯片的SPI片选控制 */
//#define TP_CS()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)	  
//#define TP_DCS() GPIO_SetBits(GPIOB,GPIO_Pin_7)

void tp_Config(void) ;
u16 TPReadX(void);
u16 TPReadY(void);
void NVIC_Configuration(void);
extern void FSMC_LCD_Init(void); 





void GPIO_Configuration5(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  /* 使能各端口时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				                 //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   GPIO_ResetBits(GPIOC , GPIO_Pin_13);
}

/****************************************************************************
* 名    称：void BSP_Init(void)
* 功    能：奋斗板初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void BSP_Init(void)
{   																		   
 // RCC_Configuration();  	       //系统时钟初始化及端口外设时钟使能
  GPIO_Configuration5();			   //状态LED1的初始化 
   SystemInit(); 
  //tp_Config();					   //SPI1 触摸电路初始化	    
  //FSMC_LCD_Init();				   //FSMC TFT接口初始化     
}


/****************************************************************************
* 名    称：void tp_Config(void)
* 功    能：TFT 触摸屏控制初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)、MISO(PA6)、MOSI(PA7) 设置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //复用模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* SPI1 触摸芯片的片选控制设置 PB7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出模式
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  
  /* 由于SPI1总线上挂接了4个外设，所以在使用触摸屏时，需要禁止其余3个SPI1 外设， 才能正常工作 */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016B片选 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                //SPI1 VS1003片选 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                 //SPI1 网络模块片选
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);							//SPI1 SST25VF016B片选置高 
  GPIO_SetBits(GPIOB, GPIO_Pin_12);							//SPI1 VS1003片选置高  
  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 网络模块片选置高 
  
   /* SPI1总线 配置 */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //全双工  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8位
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //时钟极性 空闲状态时，SCK保持低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;  //波特率控制 SYSCLK/64
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 使能 */  
  SPI_Cmd(SPI1,ENABLE);  
}

/****************************************************************************
* 名    称：unsigned char SPI_WriteByte(unsigned char data) 
* 功    能：SPI1 写函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
unsigned char SPI_WriteByte(unsigned char data) 
{ 
 unsigned char Data = 0; 

  //等待发送缓冲区空
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
  // 发送一个字节  
  SPI_I2S_SendData(SPI1,data); 

   //等待是否接收到一个字节 
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET); 
  // 获得该字节
  Data = SPI_I2S_ReceiveData(SPI1); 

  // 返回收到的字节 
  return Data; 
}  



/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/
