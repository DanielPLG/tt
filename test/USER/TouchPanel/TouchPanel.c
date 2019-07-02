/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.c
** Descriptions:            The TouchPanel application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "TouchPanel.h"
//#include "GLCD.h"
#include "Main.h"
#include <stdio.h>
#include <string.h>
#define      Control_Regist       0x858B 
#define      Control_Regist_MSB   0x85 
#define      Control_Regist_LSB   0x8B 
void floattostr(char*buf,float tmp);
 void DelayUS(vu32 cnt);
u16 chan1ReadWriteADC122(u16 ByteData);
u8 Ch1VoltLedBuf[4]={0};
u8 Ch2VoltLedBuf[4]={0};
u8 Ch3VoltLedBuf[2]={0};

u8 Ch1CurrLedBuf[4]={0};
u8 Ch2CurrLedBuf[4]={0};
u8 Ch1PowerLedBuf[4]={0};
u8 Ch2PowerLedBuf[4]={0};
u8 ch1OVPLedBuf,ch2OVPLedBuf,beepLedbuf;
u8 OTPLedBuf=0X02;
u8 LCD_ADD[]={0xf5,0x60,0xb6,0xf2,0x63,0xd3,0xd7,0x70,0xf7,0xf3};
u8 LCD_ADD_M[]={0xDF,0x86,0xEB,0xAF,0xB6,0xBD,0xFD,0x87,0xFF,0xBF};
u8 LCD_ADD_CH3[]={0xBF,0x16,0x7D,0x5F,0xD6,0xDB,0xFB,0x1E,0xFF,0xDF};

u8 LCD_ADD_DOT[]={0xfD,0x68,0xbE,0xfA,0x6B,0xdB,0xdF,0x78,0xfF,0xfB};
extern u8 BeeperFlg;
extern u8 key_ext_flg;
extern unsigned char adc_in_flg;
extern unsigned char CH1_adc_flg,CH2_adc_flg;
/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  display ;

/* DisplaySample LCD坐标上对应的ads7843采样AD值 如：LCD 坐标45,45 应该的X Y采样ADC分别为3388,920 */	
Coordinate ScreenSample[3];
/* LCD上的坐标 */
Coordinate DisplaySample[3] =   {
                                            { 45, 45 },
											{ 45, 270},
                                            { 190,190}
	                            } ;

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2   /* 差值门限 */


/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : ADS7843 SPI 初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void ADS7843_SPI_Init(void) 
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  /* DISABLE SPI1 */ 
  SPI_Cmd(SPI1, DISABLE); 
  /* SPI1 Config -------------------------------------------------------------*/ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI1, &SPI_InitStructure); 
  /* Enable SPI1 */ 
  SPI_Cmd(SPI1, ENABLE); 
} 
void floattostr(char*buf,float tmp)
{
//#ifdef HIGHER_RESOLUTION
sprintf(buf,"%.3f",tmp); 
//#else 
//sprintf(buf,"%.2f",tmp);
//#endif
}
 void AD7792_SPI_Init(void) 
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  /* DISABLE SPI1 */ 
  SPI_Cmd(SPI1, DISABLE); 
  /* SPI1 Config -------------------------------------------------------------*/ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256  ; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI1, &SPI_InitStructure); 
  /* Enable SPI1 */ 
  SPI_Cmd(SPI1, ENABLE); 
}
 void CH2AD7792_SPI_Init(void) 
{ 
  SPI_InitTypeDef  SPI_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  /* DISABLE SPI1 */ 
  SPI_Cmd(SPI2, DISABLE); 
  /* SPI1 Config -------------------------------------------------------------*/ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256  ; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI2, &SPI_InitStructure); 
  /* Enable SPI1 */ 
  SPI_Cmd(SPI2, ENABLE); 
}
 
void CH3_AD_SPI_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	/* DISABLE SPI1 */ 
	SPI_Cmd(SPI1, DISABLE); 
	/* SPI1 Config -------------------------------------------------------------*/ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b; 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64  ; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI1, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI1, ENABLE);
}

void CH3_DA_SPI_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	/* DISABLE SPI1 */ 
	SPI_Cmd(SPI1, DISABLE); 
	/* SPI1 Config -------------------------------------------------------------*/ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b; 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256  ; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI1, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI1, ENABLE);
}

void CH3_AD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* TP_CS */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ch3AD_CS(1); 
	ch3DA_CS(1);
}
void CH3_ad_da_INIT(void)
{
	CH3_AD_GPIO_Init();
	CH3_DA_SPI_Init();

}
void CH3_TLV5623_Write(u16 byte)
{
		ch3DA_CS(0); 
		ch3AD_CS(1);
		DelayUS(150);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
		/* Send SPI1 data */ 
		SPI_I2S_SendData(SPI1,byte); 
		/* Wait for SPI1 data reception */ 
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
		/* Read SPI1 received data */ 
		SPI_I2S_ReceiveData(SPI1); 
		
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
		/* Send SPI1 data */ 
		//SPI_I2S_SendData(SPI1,byte); 
		/* Wait for SPI1 data reception */ 
		//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
		/* Read SPI1 received data */ 
		//SPI_I2S_ReceiveData(SPI1); 

		DelayUS(150);
		ch3DA_CS(1); 

}

void CH3_set_Dac(u16 byte)
{
	u16 temp;
	temp=0;
	temp |=(byte<<4);
	CH3_TLV5623_Write( temp);
}

void ch3_da_test(void)
{
  CH3_TLV5623_Write(0x07F0);
}
u16 CH3_ADS7040_Write_Read(u16 byte)
{
	u16 data=0;

ch3AD_CS(0); 
DelayUS(50);
while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
/* Send SPI1 data */ 
SPI_I2S_SendData(SPI1,byte); 
/* Wait for SPI1 data reception */ 
while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
/* Read SPI1 received data */ 
data=SPI_I2S_ReceiveData(SPI1); 
data=data>>6;

//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
/* Send SPI1 data */ 
//SPI_I2S_SendData(SPI1,byte); 
/* Wait for SPI1 data reception */ 
//while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
/* Read SPI1 received data */ 
//SPI_I2S_ReceiveData(SPI1); 

DelayUS(50);
ch3AD_CS(1);
return data;

}

u16 ch3_ad_get_mean(void)
{
    int mean,i,count;
  	u16 data;
	mean=0;
	count=0;
	for(i=0;i<300;i++)
		{
	count++;
	data=CH3_ADS7040_Write_Read(0xffff);
	mean=(mean*(count-1)+data)/count;
		}
	data=mean;
}
void AD7562_io(u8 af)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE , ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_3| GPIO_Pin_4 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |GPIO_Pin_11|GPIO_Pin_12  |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0  | GPIO_Pin_5  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 |GPIO_Pin_11 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	BEEP_OFF;
	CH3_ON_DELAY(1);
	CH4_ON_DELAY(1);
    DA7562_CLK(0);
	ch2DA7562_CLK(0);
	TP_CS(1);
	ch2TP_CS(1);
}

void ADS7040_io(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  , ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 |GPIO_Pin_7 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 



	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ch3AD_CS(1);
	ADS7040_CLK(0);
   // DA7562_CLK(0);

	//ch2TP_CS(1);
}

void CH2AD8812_io(u8 af)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
	if(af)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
	else
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

/*******************************************************************************
* Function Name  : TP_Init
* Description    : ADS7843端口初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_Init(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* TP_CS */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	TP_CS(1);   
	DA7562_CS(1);
	/* TP_IRQ */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADS7843_SPI_Init(); 
	// AD7792_SPI_Init() ;
}

/*******************************************************************************
* Function Name  : DelayUS
* Description    : 延时1us
* Input          : - cnt: 延时值
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
 void DelayUS(vu32 cnt)
{
	uint16_t i;
	for(i = 0;i<cnt;i++)
	{
		uint8_t us = 12; /* 设置值为12，大约延1微秒 */    
		while (us--)     /* 延1微秒	*/
		{
			;   
		}
	}
}
 void DelayUS_ad(vu32 cnt)
{
	uint16_t i;
	for(i = 0;i<cnt;i++)
	{
		uint8_t us = 12; /* 设置值为12，大约延1微秒 */    
		while (us--)     /* 延1微秒	*/
		{
			;   
		}
	}
}
/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 向 ADS7843写数据
* Input          : - cmd: 传输的数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
	/* Wait for SPI1 Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI1,cmd); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	SPI_I2S_ReceiveData(SPI1); 
} 

static void CH2WR_CMD (uint8_t cmd)  
{ 
	// SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	/* Wait for SPI1 Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI2,cmd); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	SPI_I2S_ReceiveData(SPI2); 
} 


/*******************************************************************************
* Function Name  : RD_AD
* Description    : 读取ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回二字节数据
* Attention		 : None
*******************************************************************************/
static int RD_AD(void)  
{ 
	unsigned short buf,temp; 
	/* Wait for SPI1 Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI1,0x0000); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	temp=SPI_I2S_ReceiveData(SPI1); 
	buf=temp<<8; 
	DelayUS(1); 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 

	SPI_I2S_SendData(SPI1,0x0000); 

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 

	temp=SPI_I2S_ReceiveData(SPI1); 
	buf |= temp;  
	// buf>>=3; 
	buf&=0xffff; 	 
	return buf;
} 
static int CH2RD_AD(void)  
{ 
	unsigned short buf,temp; 
	/* Wait for SPI1 Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI2,0x0000); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	temp=SPI_I2S_ReceiveData(SPI2); 
	buf=temp<<8; 
	DelayUS(1); 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 

	SPI_I2S_SendData(SPI2,0x0000); 

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 

	temp=SPI_I2S_ReceiveData(SPI2); 
	buf |= temp;  
	// buf>>=3; 
	buf&=0xffff; 	 
	return buf;
}


void WR_AD(u8 hbye,u8 lbye,u8 idex)
{
	unsigned short buf,temp; 
	TP_CS(0); 
	DelayUS(1); 
	WR_CMD(idex); 
	DelayUS(1);
	/* Wait for SPI1 Tx buffer empty */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI1,hbye); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	temp=SPI_I2S_ReceiveData(SPI1); 
	buf=temp<<0; 
	DelayUS(1); 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 

	SPI_I2S_SendData(SPI1,lbye); 

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 

	temp=SPI_I2S_ReceiveData(SPI1); 
	DelayUS(1); 
	TP_CS(1); 
}

void floattostr1(char*buf,double tmp)
{
//#ifdef HIGHER_RESOLUTION
sprintf(buf,"%.9f",tmp); 
//#else 
//sprintf(buf,"%.2f",tmp);
//#endif
}
void floattostr2(char*buf,double tmp)
{
//#ifdef HIGHER_RESOLUTION
sprintf(buf,"%.1f",tmp); 
//#else 
//sprintf(buf,"%.2f",tmp);
//#endif
}

void WriteToReg(u8 ByteData,u8 channel) // write ByteData to the register
{
	if(channel)
	{
		TP_CS(0); 
		DelayUS(1); 
		WR_CMD(ByteData);
		DelayUS(1);
		TP_CS(1); 
	}
	else 
	{
		ch2TP_CS(0); 
		DelayUS(1); 
		CH2WR_CMD(ByteData);
		DelayUS(1);
		ch2TP_CS(1); 
	} 
}
u8 chan1ReadFromReg(void) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u8 temp;
   AD7792_DIN(1);
    TP_CS(0);
	DelayUS(120);
    temp=0;
		for(j=0; j<8; j++)
	    {
	     	AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}
			else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			
	       AD7792_CLK(1);
			DELAY_MS2(1);
		  }
	DelayUS(20);
    TP_CS(1); 
	DelayUS(80);
	return temp;
}


u8 chan2ReadFromReg(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	u8 temp;
	ch2AD7792_DIN(1);
	ch2TP_CS(0);
	DelayUS(120); 
	temp=0;
	for(j=0; j<8; j++)
	{
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
		if(!ch2AD7792_DOUT)
		{
			temp=temp<<1;
		}
		else
		{
			temp=temp<<1;
			temp=temp+0x01;
		}

		ch2AD7792_CLK(1);
		DELAY_MS2(1);
	}
	DelayUS(20);
	ch2TP_CS(1); 
	DelayUS(80);
	return temp;
}
u16 chan1ReadAD77921(void) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u16 temp;
   AD7792_DIN(1);
    TP_CS(0);
	DelayUS(320);
    temp=0;
		for(j=0; j<16; j++)
	    {
	     	AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			
	        AD7792_CLK(1);
			DELAY_MS2(1);
		  }
	DelayUS(180);
    TP_CS(1); 
	DelayUS(80);
	return temp;
}
u16 chan2ReadAD77921(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	u16 temp;
	ch2AD7792_DIN(1);
	ch2TP_CS(0);
	DelayUS(120);
	temp=0;
	for(j=0; j<16; j++)
	{
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
		if(!ch2AD7792_DOUT)
		{
			temp=temp<<1;
		}
		else
		{
			temp=temp<<1;
			temp=temp+0x01;
		}

		ch2AD7792_CLK(1);
		DELAY_MS2(1);
	}
	DelayUS(20);
	ch2TP_CS(1); 
	DelayUS(80);
	return temp;
}
u8 chan2ReadAD7792_BYTE(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	u8 temp;
	//ch2AD7792_CLK(1);
	//ch2TP_CS(0);
	DELAY_MS2(1);
	temp=0;
	for(j=0; j<8; j++)
	{
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
		if(!ch2AD7792_DOUT)
		{
			temp=temp<<1;
		}
		else
		{
			temp=temp<<1;
			temp=temp+0x01;
		}

		ch2AD7792_CLK(1);
		DELAY_MS2(1);
	}
DELAY_MS2(1);
	return temp;
}
u16 chan1ReadAD7792(void) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u16 temp;
   AD7792_DIN(0);
    TP_CS(0);
	DelayUS(120);
    temp=0;
		for(j=0; j<16; j++)
	    {
	     	AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			
	        AD7792_CLK(1);
			DELAY_MS2(1);
		  }
	DelayUS(20);
    TP_CS(1); 
	DelayUS(80);
	temp=temp&0x0fff;
	return temp;
}


u16 chan2ReadWriteADC122(u16 ByteData) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u16 temp,temp1;
   //AD7792_DIN(1);
    ch2TP_CS(0);
	DelayUS(120);
    temp=0;
	temp1=0x8000;
		for(j=0; j<16; j++)
	    {
		if((temp1&ByteData)==0)
		{		
      		ch2AD7792_DIN(0);
		}	
 		else
		{
			ch2AD7792_DIN(1);
     	}

	     	ch2AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!ch2AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			temp1=temp1>>1;
	        ch2AD7792_CLK(1);
			DELAY_MS2(1);
			//AD7792_CLK(0);
		  }
	DelayUS(20);
    ch2TP_CS(1); 
	DelayUS(80);
	temp=temp&0x0fff;
	return temp;
}
void ADS1118_init(void)  
{  

TP_CS(1);
ch2TP_CS(1);
 DELAY_MS2(1);
 	AD7792_CLK(0);
	ch2AD7792_CLK(0);
 DELAY_MS2(1);
 AD7792_DIN(0);
 ch2AD7792_DIN(0);
 DELAY_MS2(1);
} 
unsigned char ADS1118_Read(unsigned char data)   //SPI为全双工通信方式  
{  
 unsigned char i,temp,Din;  
  temp=data;  
  for(i=0;i<8;i++)  
  {  
   Din = Din<<1;  
   if(0x80&temp)  
    AD7792_DIN(1);
   else  
    AD7792_DIN(0);
    DELAY_MS2(1); 
    AD7792_CLK(1);
    DELAY_MS2(1);
   if(AD7792_DOUT)  
    Din |= 0x01;  
   DELAY_MS2(1); 
    AD7792_CLK(0);
    DELAY_MS2(1);
    temp = (temp<<1);  
  }  
  return Din;  
}  
unsigned char ADS7040_Read(unsigned short data)   //SPI为全双工通信方式  
{  
 unsigned short i,temp,Din; 
 Din=0;
  temp=data;  
  ch3AD_CS(0);
  DelayUS(150);
  for(i=0;i<10;i++)  
  {  
   Din = Din<<1;  
   if(0x200&temp)  
    ADS7040_DIN(1);
   else  
    ADS7040_DIN(0);
    DELAY_MS3(1); 
    ADS7040_CLK(1);
    DELAY_MS3(1);
   if(ADS7040_DOUT)  
    Din |= 0x01;  
   DELAY_MS3(1); 
    ADS7040_CLK(0);
    DELAY_MS3(1);
    temp = (temp<<1);  
  }  
  DelayUS(150);
  ch3AD_CS(1);
  return Din;  
}

unsigned char ADS1118_Read_CH2(unsigned char data)   //SPI为全双工通信方式  
{  
 unsigned char i,temp,Din;  
  temp=data;  
  for(i=0;i<8;i++)  
  {  
   Din = Din<<1;  
   if(0x80&temp)  
    ch2AD7792_DIN(1);
   else  
    ch2AD7792_DIN(0);
    DELAY_MS2(1); 
    ch2AD7792_CLK(1);
    DELAY_MS2(1);
   if(ch2AD7792_DOUT)  
    Din |= 0x01;  
   DELAY_MS2(1); 
    ch2AD7792_CLK(0);
    DELAY_MS2(1);
    temp = (temp<<1);  
  }  
  return Din;  
}
float ADS1118_Voltage;  
unsigned int ADS1118_Get_Voltage_CH2(u8 channel)  
{  
 unsigned int i=0;  
 unsigned char Data_REG_H,Data_REG_L,REG_H,temp1,temp2;  
 unsigned int Data_REG,test;  
 adc_in_flg=1;
if(channel==0)
{
	REG_H=Control_Regist_MSB;
}
else
{
REG_H=0xB5;
}

// if(channel==0)
 {
 	ch2TP_CS(0);
	key_ext_flg=0;
	DelayUS_ad(120);
 	 ADS1118_Read_CH2(REG_H);  
 	ADS1118_Read_CH2(Control_Regist_LSB);  

 	 ADS1118_Read_CH2(REG_H);  
 	ADS1118_Read_CH2(Control_Regist_LSB);  
	DelayUS_ad(120);
	ch2TP_CS(1);
	// while((AD7792_DOUT)&&(i<100000)) i++;  
	 DELAY_MS2(8); 
	 ch2TP_CS(0);
	  
	 DelayUS_ad(120);
	 Data_REG_H=ADS1118_Read_CH2(REG_H);  
	 Data_REG_L=ADS1118_Read_CH2(Control_Regist_LSB);  
 /*  if(key_ext_flg)
   	{
   	CH1_adc_flg=1;
	key_ext_flg=0;
	 adc_in_flg=0;
	return 0;
   	}*/
 	temp1= ADS1118_Read_CH2(REG_H);  
 	temp2=ADS1118_Read_CH2(Control_Regist_LSB);  


	 if(channel==0)
	 {
           if(temp1==0x05&&temp2==0x8a)
           	{
			CH1_adc_flg=0;
		   }
		   else
		   {
		   	CH1_adc_flg=1;
		   }
	 }
	 else
	 {
	            if(temp1==0x35&&temp2==0x8a)
           	{
			CH1_adc_flg=0;
		   }
		 else
		 {
		 	CH1_adc_flg=1;
		 }
	 }
	DelayUS_ad(120);
	ch2TP_CS(1);
	 Data_REG=(Data_REG_H<<8)+Data_REG_L;  

 }

   if(key_ext_flg)
   	{
   	CH1_adc_flg=1;
	key_ext_flg=0;
   	}
   adc_in_flg=0;
return Data_REG;
}
unsigned int ADS1118_Get_Voltage(u8 channel)  
{  
 unsigned int i=0;  
 unsigned char Data_REG_H,Data_REG_L,REG_H,temp1,temp2;  
 unsigned int Data_REG;  
 adc_in_flg=1;
if(channel==0)
{
	REG_H=Control_Regist_MSB;
}
else
{
REG_H=0xB5;
}


// if(channel==0)
 {
 	TP_CS(0);
	key_ext_flg=0;
	DelayUS_ad(120);
 	 ADS1118_Read(REG_H);  
 	ADS1118_Read(Control_Regist_LSB);  
	 ADS1118_Read(REG_H); 
	 ADS1118_Read(Control_Regist_LSB);  
	DelayUS_ad(120);
	TP_CS(1);
	// while((AD7792_DOUT)&&(i<100000)) i++;  
	 DELAY_MS2(8); 
	 TP_CS(0);
	 
	 DelayUS_ad(120);
	 Data_REG_H=ADS1118_Read(REG_H);  
	 Data_REG_L=ADS1118_Read(Control_Regist_LSB);  
/*  if(key_ext_flg)
   	{
   	CH1_adc_flg=1;
	key_ext_flg=0;
	 adc_in_flg=0;
	return 0;
   	}*/
	temp1= ADS1118_Read(REG_H);  
	 temp2=ADS1118_Read(Control_Regist_LSB);  
	 if(channel==0)
	 {
           if(temp1==0x05&&temp2==0x8a)
           	{
			CH1_adc_flg=0;
		   }
		   else
		   {
		   	CH1_adc_flg=1;
		   }
	 }
	 else
	 {
	            if(temp1==0x35&&temp2==0x8a)
           	{
			CH1_adc_flg=0;
		   }
		 else
		 {
		 	CH1_adc_flg=1;
		 }
	 }
	 //CH1_adc_flg
	DelayUS_ad(120);
	TP_CS(1);
	 Data_REG=(Data_REG_H<<8)+Data_REG_L;  

 }

   if(key_ext_flg)
   	{
   	CH1_adc_flg=1;
	key_ext_flg=0;
   	}
   adc_in_flg=0;
return Data_REG;
}  
u16 chan1ReadWriteADC122(u16 ByteData) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u16 temp,temp1;
   //AD7792_DIN(1);
    TP_CS(0);
	DelayUS(120);
    temp=0;
	temp1=0x8000;
		for(j=0; j<16; j++)
	    {
		if((temp1&ByteData)==0)
		{		
      		AD7792_DIN(0);
		}	
 		else
		{
			AD7792_DIN(1);
     	}

	     	AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			temp1=temp1>>1;
	        AD7792_CLK(1);
			DELAY_MS2(1);
			//AD7792_CLK(0);
		  }
	DelayUS(20);
    TP_CS(1); 
	DelayUS(80);
	temp=temp&0x0fff;
	return temp;
}
u16 chan2ReadAD7792(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	u16 temp;
	ch2AD7792_DIN(1);
	ch2TP_CS(0);
	DelayUS(120);
	temp=0;
	for(j=0; j<16; j++)
	{
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
		if(!ch2AD7792_DOUT)
		{
			temp=temp<<1;
		}
		else
		{
			temp=temp<<1;
			temp=temp+0x01;
		}

		ch2AD7792_CLK(1);
		DELAY_MS2(1);
	}
	DelayUS(20);
	ch2TP_CS(1); 
	DelayUS(80);
	return temp;
}
void chan2WriteToReg1(u8 ByteData) // write ByteData to the register
{
	u8 temp;
	int i;	
	ch2TP_CS(0);
	 DelayUS(120);
	temp=0x80;
	for(i=0;i<8;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		ch2AD7792_DIN(0);
		}	
 		else
		{
			ch2AD7792_DIN(1);
     	}
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
	   	ch2AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}
 	DelayUS(20);
   ch2TP_CS(1);
	ch2AD7792_DIN(1); 
	DelayUS(80);
}
void chan2WriteToReg2(u8 ByteData) // write ByteData to the register
{
	u8 temp;
	int i;	
	//ch2TP_CS(0);
	//ch2AD7792_CLK(1);
	 DELAY_MS2(1);
	temp=0x80;
	for(i=0;i<8;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		ch2AD7792_DIN(0);
		}	
 		else
		{
			ch2AD7792_DIN(1);
     	}
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
	   	ch2AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}
	DELAY_MS2(1);
}
void chan1WriteToReg1(u8 ByteData) // write ByteData to the register
{
	u8 temp;
	int i;	
	TP_CS(0);
	 DelayUS(120);
	temp=0x80;
	for(i=0;i<8;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		AD7792_DIN(0);
		}	
 		else
		{
			AD7792_DIN(1);
     	}
		AD7792_CLK(0);
		DELAY_MS2(1);
	   	AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}
 	 DelayUS(120);

    TP_CS(1);
	AD7792_DIN(1);
	DelayUS(80);
}
u8 chan2ReadFromReg1(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	u8 temp;
	ch2AD7792_DIN(1);
	ch2TP_CS(0);
	DelayUS(120); 
	temp=0;
	for(j=0; j<8; j++)
	{
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
		if(!ch2AD7792_DOUT)
		{
			temp=temp<<1;
		}
		else
		{
			temp=temp<<1;
			temp=temp+0x01;
		}

		ch2AD7792_CLK(1);
		DELAY_MS2(1);
	}
	DelayUS(20);
	ch2TP_CS(1); 
	DelayUS(80);
	return temp;
}
u8 chan1ReadFromReg1(void) // nByte is the number of bytes which need to be read
{
	int i,j;
   	u8 temp;
   AD7792_DIN(1);
    TP_CS(0);
	DelayUS(120);
    temp=0;
		for(j=0; j<8; j++)
	    {
	     	AD7792_CLK(0);
			DELAY_MS2(1);
	     	if(!AD7792_DOUT)
	     	{
				temp=temp<<1;
		 	}
			else
		 	{
				temp=temp<<1;
		 		temp=temp+0x01;
			}
			
	       AD7792_CLK(1);
			DELAY_MS2(1);
		  }
	DelayUS(20);
    TP_CS(1); 
	DelayUS(80);
	return temp;
}
void chan1WriteToReg(unsigned short ByteData) // write ByteData to the register
{
	unsigned short temp;
	int i;	
	TP_CS(0);
	 DelayUS(120);

	temp=0x8000;
	for(i=0;i<16;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		AD7792_DIN(0);
		}	
 		else
		{
			AD7792_DIN(1);
     	}
		AD7792_CLK(0);
		DELAY_MS2(1);
	   	AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}
 	 DelayUS(20);

    TP_CS(1);
	AD7792_DIN(1);
	DelayUS(80);
}

void  chan1rst(u8 ByteData) 
{
	u8 temp;
	int i;	
	temp=0x80;
	for(i=0;i<8;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		AD7792_DIN(0);
		}	
 		else
		{
			AD7792_DIN(1);
     	}
		AD7792_CLK(0);
		DELAY_MS2(1);
	   	AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}

}
void  chan2rst(u8 ByteData) 
{
	u8 temp;
	int i;	
	temp=0x80;
	for(i=0;i<8;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		ch2AD7792_DIN(0);
		}	
 		else
		{
			ch2AD7792_DIN(1);
     	}
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
	   	ch2AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}

}
void chan2WriteToReg(unsigned short ByteData) // write ByteData to the register
{
	unsigned short temp;
	int i;	
	ch2TP_CS(0);
	 DelayUS(120);
	temp=0x8000;
	for(i=0;i<16;i++)
	{
 		if((temp & ByteData)==0)
		{		
      		ch2AD7792_DIN(0);
		}	
 		else
		{
			ch2AD7792_DIN(1);
     	}
		ch2AD7792_CLK(0);
		DELAY_MS2(1);
	   	ch2AD7792_CLK(1);
		DELAY_MS2(1);
 		temp=temp>>1;
	}
 	DelayUS(20);
   ch2TP_CS(1);
	ch2AD7792_DIN(1); 
	DelayUS(80);
}


int Read_AD7792(void)
{
	u16 buf,temp; 
	temp=chan1ReadFromReg(); 
	buf=temp<<8;  
	DELAY_MS2(1);
	temp=chan1ReadFromReg(); 
	buf |= temp;  
	buf&=0xffff; 	 
	return buf; 
} 


int CH2Read_AD7792(void)
{
	u16 buf,temp; 
	temp=chan2ReadFromReg(); 
	buf=temp<<8;  
	DELAY_MS2(1);
	temp=chan2ReadFromReg(); 
	buf |= temp;  
	buf&=0xffff; 	 
	return buf; 
} 


int ReadFromReg(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	unsigned char temp;
	TP_CS(0);
	DelayUS(1); 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI1,0x0000); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	temp=SPI_I2S_ReceiveData(SPI1);
	DelayUS(1);
	TP_CS(1); 
	return temp;
}


int CH2ReadFromReg(void) // nByte is the number of bytes which need to be read
{
	int i,j;
	unsigned char temp;
	ch2TP_CS(0);
	DelayUS(1); 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* Send SPI1 data */ 
	SPI_I2S_SendData(SPI2,0x0000); 
	/* Wait for SPI1 data reception */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Read SPI1 received data */ 
	temp=SPI_I2S_ReceiveData(SPI2);
	DelayUS(1);
	ch2TP_CS(1); 
	return temp;
}

void AD7792_RST(void)
{
	int i; 
	AD7792_CLK(1);
	DelayUS(120);
	TP_CS(0);
	DelayUS(10);
	 for(i=0;i<4;i++)
	 {
	 	chan1rst(0xff); 
	 }
	  DelayUS(20);
	  TP_CS(1);
	  DELAY_MS2(1);
}


void CH2AD7792_RST(void)
{
	int i;
	ch2AD7792_CLK(1);
	DelayUS(120);
	ch2TP_CS(0);
	DelayUS(10);
	 for(i=0;i<4;i++)
	 {
	 	chan2rst(0xff); 
	 }
	  DelayUS(20);
	  ch2TP_CS(1);
	  DELAY_MS2(1);
}
/*******************************************************************************
* Function Name  : Read_X
* Description    : 读取ADS7843通道X+的ADC值 
* Input          : None
* Output         : None
* Return         : ADS7843返回通道X+的ADC值
* Attention		 : None
*******************************************************************************/

int Read_X1(u8 reg)  
{  
	int i; 
	// TP_CS(0); 
	DelayUS(1); 
	chan1WriteToReg1(reg);
	DelayUS(1); 
	i=chan1ReadAD77921();
	DelayUS(1); 
	// TP_CS(1); 
	return i;
}
int CH2Read_X1(u8 reg)  
{  
  int i; 
  //ch2TP_CS(0); 
  DelayUS(1); 
  chan2WriteToReg1(reg); 
  DelayUS(1); 
  i=chan2ReadAD77921();
  DelayUS(1); 
  //ch2TP_CS(1); 
  return i;  
}
int Read_X(u8 reg)  
{  
	unsigned short i; 
	// TP_CS(0); 
	//DelayUS(1); 
	//chan1WriteToReg(reg);
	DelayUS(1); 
	i=chan1ReadAD7792();
	i &=0x0fff;
	DelayUS(1); 
	// TP_CS(1); 
	return i;
} 


int CH2Read_X(u8 reg)  
{  
  unsigned short i;  
  //ch2TP_CS(0); 
 // DelayUS(1); 
 // chan2WriteToReg(reg); 
  DelayUS(1); 
  i=chan2ReadAD7792();
  i &=0x0fff;
  DelayUS(1); 
  //ch2TP_CS(1); 
  return i;  
}
/*******************************************************************************
* Function Name  : Read_Y
* Description    : 读取ADS7843通道Y+的ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回通道Y+的ADC值
* Attention		 : None
*******************************************************************************/
int Read_Y(void)  
{  
	int i; 
	TP_CS(0); 
	DelayUS(1); 
	WR_CMD(CHY); 
	DelayUS(1); 
	i=RD_AD(); 
	TP_CS(1); 
	return i;     
} 










void DAC7562_WR(u16 val,u8 channel)
{
	unsigned int  temp,i,ad;
	temp=0x800000;

	DA7562_LDAC(1);
	DA7562_CS(0);  
	DelayUS(1);
	if(channel) 
	{
     ad=0x010000;
	}
	else
	{
     ad=0x000000;
	}
    val=val<<4;
	ad |=(unsigned int)val;
	for(i=0;i<24;i++)
	{
		DELAY_MS2(1); 
		DA7562_CLK(0);
		if((ad&temp)==0)
			DA7562_DIN(0);
		else
			DA7562_DIN(1);
		DELAY_MS2(1);		
		DA7562_CLK(1);
		DELAY_MS2(1);
		DA7562_CLK(0);
		ad=(ad<<1);
	}
	DELAY_MS2(1);


	DA7562_CS(1);
	DELAY_MS2(1);

	DA7562_LDAC(0);
	DELAY_MS2(1);
	DA7562_LDAC(1);
}


void CH2DAC7562_WR(u16 val,u8 channel)
{
	unsigned int  temp,i,ad;
	temp=0x800000;

	ch2DA7562_LDAC(1);
	ch2DA7562_CS(0);  
	DELAY_MS2(1);
	//DelayUS(1);
	if(channel) 
	{
     ad=0x010000;
	}
	else
	{
     ad=0x000000;
	}

	val=val<<4;
	ad |=(unsigned int)val;
	for(i=0;i<24;i++)
	{
		DELAY_MS2(1); 
		ch2DA7562_CLK(0);
		if((ad&temp)==0)
			ch2DA7562_DIN(0);
		else
			ch2DA7562_DIN(1);
		DELAY_MS2(1);		
		ch2DA7562_CLK(1);
		DELAY_MS2(1);
		ch2DA7562_CLK(0);
		ad=(ad<<1);
	}
	DELAY_MS2(1);


	ch2DA7562_CS(1);
	DELAY_MS2(1);

	ch2DA7562_LDAC(0);
	DELAY_MS2(1);
	ch2DA7562_LDAC(1);
}


void DAC_Set(u16 val,u8 channel)
{
	OS_CPU_SR  cpu_sr ;
	//OS_ENTER_CRITICAL();

	DAC7562_WR( val, channel);

	//OS_EXIT_CRITICAL();	
}


void CH2DAC_Set(u16 val,u8 channel)
{
	OS_CPU_SR  cpu_sr ;
//	OS_ENTER_CRITICAL();

	CH2DAC7562_WR( val, channel);

//	OS_EXIT_CRITICAL();	
}

extern u8 recall_test;
void func_set(float value,u8 channel,u8 chan_stat )
{
	float temp;
	float div=0;
	float tmp=0;
	
    //key_vale=KEY_NONE;
	//key_stat=1;
	recall_test=1;
	if(chan_stat==CH1_SEL)
	{
		if(channel)
		{
			tmp=(((value*PARA_K[CH1_VOLT_SET_CAL ].a+PARA_K[CH1_VOLT_SET_CAL ].b)/40)*MAX_OF_DAC );
			temp=(u16)tmp;
		}
		else
		{
			tmp=(((value*PARA_K[CH1_CURR_SET_CAL ].a+PARA_K[CH1_CURR_SET_CAL ].b)/8.1705)*MAX_OF_DAC );
			temp=(u16)tmp;
		}
		if(channel==0)
		{
			DAC_Set(temp,0);
	
		}
		else
		{
			DAC_Set(temp,1);
	
		}
	}
	else
	{
		if(channel)
		{
			tmp=(((value*PARA_K[CH2_VOLT_SET_CAL ].a+PARA_K[CH2_VOLT_SET_CAL ].b)/40)*MAX_OF_DAC );
			temp=(u16)tmp;
		}
		else
		{
			tmp=(((value*PARA_K[CH2_CURR_SET_CAL ].a+PARA_K[CH2_CURR_SET_CAL ].b)/8.1705)*MAX_OF_DAC );
			temp=(u16)tmp;
		}
		if(channel==0)
		{
			CH2DAC_Set(temp,0);
	
		}
		else
		{
			CH2DAC_Set(temp,1);

		}
	}
	//RCC_Configuration();
}
void func_set_ch3(float value )
{
	u16 temp;
	float div=0;
	float tmp=0;

			
			tmp=(((value*PARA_K[CH3_VOLT_SET_CAL ].a+PARA_K[CH3_VOLT_SET_CAL ].b)/6.7)*MAX_OF_DA_AD_CH3 );
			temp=(u16)tmp;
	

			CH3_set_Dac(temp);
	
		
	

}

void ad7792_change(u8 channel,u8 val)
{
	AD7562_io(1);
	AD7792_SPI_Init();
	ADCLK_Inint();
	WriteToReg(0x10,val); //write to Communication register.The next step is writing to Configuration register.
	if(channel)
		WriteToReg(0x10,val); //set the Configuration bipolar mode.Gain=1.
	else
		WriteToReg(0x11,val); //set the Configuration bipolar mode.Gain=1.
	WriteToReg(0x00,val);
	RCC_Configuration();
	AD7562_io(0);
}

unsigned short  advalue=0;
int ADC_Get(u8 channel)
{
	 int temp;
	 int overtime;
     unsigned short  cmd;
	 if(channel==1)
	 cmd=0x0800;
	 else
	 cmd=0x0000;
      //chan1WriteToReg(cmd);

	 DELAY_MS2(20);
	 overtime=0;
    temp=chan1ReadWriteADC122(cmd);
	// temp=Read_X(RDATA_REG );
	 return temp;
}
int ADC_Get_AD7792(u8 channel)
{
	 int temp;
	 int overtime;
	 chan1WriteToReg1(0x10);
	 chan1WriteToReg1(0x10);
	 chan1WriteToReg1(channel);

	 chan1WriteToReg1(0x08);
	 chan1WriteToReg1(0x20);//set the mode register as single conversion mode.
	 chan1WriteToReg1(0x0A);
	 chan1WriteToReg1(0x40);
	 CH1_adc_flg=0;
	 overtime=0;
	 if (channel)
	 {
		 while((chan1ReadFromReg1()!=0x01))//wait for the end of convertion by polling the status register RDY bit	 
	   		{
	   		     overtime++;
	   		     if(overtime>30)
	   		     	{
	   		     	CH1_adc_flg=1;
	   		     	return 0;		
	   		     	}
	   		     chan1WriteToReg1(0x40);
	   		}	
		}
	 else
	 {
	      while((chan1ReadFromReg1()))//wait for the end of convertion by polling the status register RDY bit	 
	   		{
	   		     overtime++;
	   		     if(overtime>30)
	   		     	{
	   		     	CH1_adc_flg=1;
	   		     	return 0;		
	   		     	}	  
	   		     chan1WriteToReg1(0x40);
	   		}		 	
	 }
	 temp=Read_X1(RDATA_REG );
	 return temp;
}
void AD7792_WaitRdyGoLow(void)
{
    while( ch2AD7792_DOUT )
    {
        ;
    }
}
int CH2ADC_Get_Ad7792(u8 channel)
{
	 int temp;
	 int ch2overtime;
	 chan2WriteToReg1(0x10);
	 chan2WriteToReg1(0x10);
	 chan2WriteToReg1(channel);

	 chan2WriteToReg1(0x08);
	 chan2WriteToReg1(0x20);
	 chan2WriteToReg1(0x0A);	

	 chan2WriteToReg1(0x40);
	 ch2overtime=0;
	CH1_adc_flg=0;
	//AD7792_WaitRdyGoLow();
	 if(channel)
	 {
	
	 	while((chan2ReadFromReg1()!=0x01))//wait for the end of convertion by polling the status register RDY bit	 
		 {
		     ch2overtime++;
	   		     if(ch2overtime>30)
	   		     	{
	   		     	CH1_adc_flg=1;
	   		     	return 0;		
	   		     	}	  
			  chan2WriteToReg1(0x40);
		 }	 
	 }
	else
	{
	
	 	 while((chan2ReadFromReg1()))//wait for the end of convertion by polling the status register RDY bit	 
		 {
		     ch2overtime++;
	   		     if(ch2overtime>30)
	   		     	{
	   		     	CH1_adc_flg=1;
	   		     	return 0;		
	   		     	}  
			  chan2WriteToReg1(0x40);
		 }	 	
	}
	 temp=CH2Read_X1(RDATA_REG);
	 return temp;
}
int CH2ADC_Get(u8 channel)
{
	 int temp;
	 int overtime;
     unsigned short  cmd;
	 if(channel==1)
	 cmd=0x0800;
	 else
	 cmd=0x0000;
      //chan1WriteToReg(cmd);

	 DELAY_MS2(20);
	 overtime=0;
    temp=chan2ReadWriteADC122(cmd);
	// temp=Read_X(RDATA_REG );
	 return temp;
}

void ADC_Start(void)
{

	AD7792_RST();
	DELAY_MS2(1);
	CH2AD7792_RST();		   		
	DELAY_MS2(1);
}




void SendBit_1621(unsigned char data,unsigned char cnt)  //data 的高 cnt 位写入 HT1621，高位在前
{
    unsigned char i;
//	key_ext_flg=1;
    for(i =0; i <cnt; i ++)
    {
    if((data&0x80)==0) Ht1621_Dat(0);
    else Ht1621_Dat(1);
    Ht1621_WR(0);
    //_nop();
	DELAY_MS(1);
    //wr=1;
	Ht1621_WR(1);
	DELAY_MS(1);
    data<<=1;
    }
}
void SendDataBit_1621(unsigned char data,unsigned char cnt)  //data 的低 cnt 位写入 HT1621，低位在前
 {
    unsigned char i;
	//key_ext_flg=1;
    for(i =0; i <cnt; i ++)
    {
        if((data&0x01)==0) Ht1621_Dat(0);
        else Ht1621_Dat(1);
        Ht1621_WR(0);
        DELAY_MS(1);
        Ht1621_WR(1);
		DELAY_MS(1);
        data>>=1;
    }
  }

void SendCmd(unsigned char command)
{
    //cs=0;
  //  key_ext_flg=1;
	Ht1621_CS0(0);
	DelayUS(20);
    SendBit_1621(0x80,3); //写入标志码"100"
    SendBit_1621(command,9); //写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
   // cs=1;
    DelayUS(20);
	Ht1621_CS0(1);
}
void SendCmd1(unsigned char command)
{
    //cs=0;
    //key_ext_flg=1;
	Ht1621_CS1(0);
	DelayUS(20);
    SendBit_1621_CH2(0x80,3); //写入标志码"100"
    SendBit_1621_CH2(command,9); //写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
   // cs=1;
    DelayUS(20);
	Ht1621_CS1(1);
}
void Write_1621(unsigned char addr,unsigned char data)
{
   // cs=0;
  // key_ext_flg=1;
	Ht1621_CS0(0);
    DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    SendDataBit_1621(data,4); //写入 data 的低 4 位
    //cs=1;
    DelayUS(20);
	Ht1621_CS0(1);
}
void WriteAll_1621(unsigned char addr,unsigned char *p,unsigned char cnt)
{
    unsigned char i;
    //cs=0;
   // key_ext_flg=1;
	Ht1621_CS0(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,8);
    }
   // cs=1;
   DelayUS(20);
	Ht1621_CS0(1);
}
void WriteAll_1621_CH2(unsigned char addr,unsigned char *p,unsigned char cnt)
{
    unsigned char i;
	OS_CPU_SR  cpu_sr; 
    //cs=0;
    //OS_ENTER_CRITICAL();
   // key_ext_flg=1;
	Ht1621_CS1(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,4);
    }
   // cs=1;
    DelayUS(20);
	Ht1621_CS1(1);
	//OS_EXIT_CRITICAL();
}
void WriteAll_1621_CH1(unsigned char addr,unsigned char *p,unsigned char cnt)
{
    unsigned char i;
	OS_CPU_SR  cpu_sr ;
	//OS_ENTER_CRITICAL();
    //cs=0;
     //  key_ext_flg=1;
	Ht1621_CS0(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,4);
    }
   // cs=1;
   DelayUS(20);
	Ht1621_CS0(1);
	//OS_EXIT_CRITICAL();
}
void WriteAll_1621Byte_CH1(unsigned char addr,unsigned char *p,unsigned char cnt)
{
	OS_CPU_SR  cpu_sr ;
    unsigned char i;
	//OS_ENTER_CRITICAL();
    //cs=0;
   // key_ext_flg=1;
	Ht1621_CS0(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,8);
    }
   // cs=1;
   DelayUS(20);
	Ht1621_CS0(1);
	//OS_EXIT_CRITICAL();
}
void WriteAll_1621Byte_CH2(unsigned char addr,unsigned char *p,unsigned char cnt)
{
	OS_CPU_SR  cpu_sr; 
    unsigned char i;
	//key_ext_flg=1;
    //cs=0;
    //OS_ENTER_CRITICAL();
	Ht1621_CS1(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,8);
    }
   // cs=1;
   DelayUS(20);
	Ht1621_CS1(1);
	//OS_EXIT_CRITICAL();
}
void WriteAll_1621CH2(unsigned char addr,unsigned char *p,unsigned char cnt)
{
	OS_CPU_SR  cpu_sr; 
    unsigned char i;
	//key_ext_flg=1;
    //cs=0;
	Ht1621_CS1(0);
	DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    for(i =0; i <cnt; i ++,p++)  //连续写入数据
    {
        SendDataBit_1621(*p,4);
    }
   // cs=1;
   DelayUS(20);
	Ht1621_CS1(1);
}
void SendBit_1621_CH2(unsigned char data,unsigned char cnt)  //data 的高 cnt 位写入 HT1621，高位在前
{
	
    unsigned char i;
	//key_ext_flg=1;
    for(i =0; i <cnt; i ++)
    {
    if((data&0x80)==0) Ht1621_Dat(0);
    else Ht1621_Dat(1);
    Ht1621_WR(0);
    //_nop();
	DELAY_MS(1);
    //wr=1;
	Ht1621_WR(1);
    data<<=1;
    }
}
void SendDataBit_1621_CH2(unsigned char data,unsigned char cnt)  //data 的低 cnt 位写入 HT1621，低位在前
 {
    unsigned char i;
	//key_ext_flg=1;
    for(i =0; i <cnt; i ++)
    {
        if((data&0x01)==0) Ht1621_Dat(0);
        else Ht1621_Dat(1);
        Ht1621_WR(0);
        DELAY_MS(1);
        Ht1621_WR(1);
        data>>=1;
    }
  }

void SendCmd_CH2(unsigned char command)
{
    //cs=0;
   // key_ext_flg=1;
	Ht1621_CS1(0);
	DelayUS(20);
    SendBit_1621(0x80,3); //写入标志码"100"
    SendBit_1621(command,9); //写入 9 位数据,其中前 8 位为 command 命令,最后 1 位任意
   // cs=1;
   DelayUS(20);
	Ht1621_CS1(1);
}
void Write_1621_CH2(unsigned char addr,unsigned char data)
{
   // cs=0;
   //key_ext_flg=1;
	Ht1621_CS1(0);
   DelayUS(20);
    SendBit_1621(0xa0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    SendDataBit_1621(data,4); //写入 data 的低 4 位
    //cs=1;
    DelayUS(20);
	Ht1621_CS1(1);
}

u8 ReadHT1621Byte(unsigned char addr,unsigned char cnt) // nByte is the number of bytes which need to be read
{
	int i,j;
	u8 temp;
	//key_ext_flg=1;
	//ch2AD7792_DIN(1);
	Ht1621_RD(1);
	Ht1621_CS0(0);
	DelayUS(20);
	SendBit_1621(0xc0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    Ht1621_Dat_IN();
	temp=0;
	for(j=0; j<cnt; j++)
	{
		Ht1621_RD(0);
		//Ht1621_RD(0);
		DELAY_MS(1);
		 temp = (temp>>1);  
		   if(Ht1621_DatIn)  
   			 temp |= 0x08;  

   			
		/*if(!Ht1621_DatIn)
		{
			temp=temp1;
			 temp>>=1;
		}
		else
		{
		      
			temp=temp<<1;
			temp=temp+0x01;
			
		}*/

		Ht1621_RD(1);
		DELAY_MS(1);
	}
	DelayUS(20);
	Ht1621_CS0(1); 
	DelayUS(80);
	Ht1621_Dat_OUT();
	return temp;
}

u8 ReadHT1621ByteCH2(unsigned char addr,unsigned char cnt) // nByte is the number of bytes which need to be read
{
	int i,j;
	u8 temp;
	//ch2AD7792_DIN(1);
	//key_ext_flg=1;
	Ht1621_RD(1);
	  Ht1621_WR(1);
	Ht1621_CS1(0);
	DelayUS(20);
	SendBit_1621(0xc0,3); //写入标志码"101"
    SendBit_1621(addr<<2,6);  //写入 6 位 addr
    Ht1621_Dat_IN();
	temp=0;
	for(j=0; j<cnt; j++)
	{
		Ht1621_RD(0);
		//Ht1621_RD(0);
		DELAY_MS(1);
		 temp = (temp>>1);  
		   if(Ht1621_DatIn)  
   			 temp |= 0x08;  

   			
		/*if(!Ht1621_DatIn)
		{
			temp=temp1;
			 temp>>=1;
		}
		else
		{
		      
			temp=temp<<1;
			temp=temp+0x01;
			
		}*/

		Ht1621_RD(1);
		DELAY_MS(1);
	}
	DelayUS(20);
	Ht1621_CS1(1); 
	DelayUS(80);
	Ht1621_Dat_OUT();
	return temp;
}
unsigned char buffer[32];

unsigned char buffer1[32];

void read_ht1621_all(void)
{
  unsigned char i;
	//key_ext_flg=1;
  	for(i=0;i<32;i++)
	{
        buffer[i]=ReadHT1621Byte(i,4) ;
	}
  	for(i=0;i<32;i++)
	{
        buffer1[i]=ReadHT1621ByteCH2(i,4) ;
	}
}

void write_ht1621_all(void)
{
 	unsigned char i;
	//key_ext_flg=1;
	Ht1621_CS1(1); 
	Ht1621_CS0(1);
    //buffer[0]=0x21;buffer[1]=0x43;buffer[2]=0x65;buffer[3]=0x87;a[4]=0xa9;
   // _pac=0; //将端口设为输出
    SendCmd(BIAS); //设置偏压和占空比
    SendCmd(SYSEN);  //打开系统振荡器
    SendCmd(LCDON);  //打开 LCD 偏压发生器
    for(i=0;i<32;i++)
    	{
	WriteAll_1621_CH1(i,&buffer[i],1); 
    	}
    Ht1621_CS0(1); 
    SendCmd_CH2(BIAS); //设置偏压和占空比
    SendCmd_CH2(SYSEN);  //打开系统振荡器
    SendCmd_CH2(LCDON);  //打开 LCD 偏压发生器
	
    for(i=0;i<32;i++)
    	{
	WriteAll_1621_CH2(i,&buffer1[i],1); 
    	}
}
void HT1621_Disp_Cal_volt(unsigned char ch,double val)
{
	unsigned char i,buf[10];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
    if(val<10)
   {
	if(val>9.996)
		val=10;
   } 
  floattostr1(buffer,val);
  if(val<10)
  {
   buf[0]=LCD_ADD[0]; 
   j=buffer[0]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[2]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[3]=LCD_ADD[j];

   j=buffer[4]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[5]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[7]%0x30;
   buf[7]=LCD_ADD[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD[j]; 
   j=buffer[1]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];

   j=buffer[5]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[7]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[8]%0x30;
   buf[7]=LCD_ADD[j];   
  }
  
  //OS_ENTER_CRITICAL();
  if(ch==0)
  {
  	for(i=0;i<4;i++)
  	Ch1VoltLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(0,Ch1VoltLedBuf,4);

  	for(i=0;i<4;i++)
  	Ch2VoltLedBuf[i]=buf[i+4];
    WriteAll_1621Byte_CH2(0,Ch2VoltLedBuf,4);
  }
  else
  {
  	for(i=0;i<4;i++)
  	Ch2VoltLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH2(0,buf,4);
  }
 // OS_EXIT_CRITICAL()
}

void HT1621_Disp_Save_count(unsigned char val)
{
	unsigned char i,buf[10];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
     
     j=val;
    buf[0]=LCD_ADD_M[j];
	
    WriteAll_1621Byte_CH2(24,buf,1);

 // OS_EXIT_CRITICAL()
}
void HT1621_Clear_Save_count(void)
{
	unsigned char i,buf[10];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
     

    buf[0]=0x00;
	
    WriteAll_1621Byte_CH2(24,buf,1);

 // OS_EXIT_CRITICAL()
}

void HT1621_Disp_CH3_volt(double val)
{
	unsigned char i,buf[10];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
    if(val<10)
   {
	if(val>9.996)
		val=10;
   } 
  floattostr2(buffer,val);
  if(val<1&&val>0.95)
  	val=1;
  if(val<1)
  {
   buf[1]=LCD_ADD_CH3[0]; 
   j=buffer[2]%0x30;
   buf[0]=LCD_ADD_CH3[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[1]=LCD_ADD_CH3[j]; 
   j=buffer[2]%0x30;
   buf[0]=LCD_ADD_CH3[j];

  }
  


  	for(i=0;i<2;i++)
  	Ch3VoltLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH2(27,Ch3VoltLedBuf,2);


 // OS_EXIT_CRITICAL()
}


void HT1621_Cal_curret_set(unsigned char ch,double val)
{
	unsigned char i,buf[10];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
    if(val<1)
  {
	 if(val>0.9995)
	 {
		val=1;
	 }
  }
  floattostr1(buffer,val);
  if(val<1)
  {
   buf[0]=LCD_ADD_DOT[0]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];

   j=buffer[5]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[7]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[8]%0x30;
   buf[7]=LCD_ADD[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD_DOT[j]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];

   j=buffer[5]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[7]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[8]%0x30;
   buf[7]=LCD_ADD[j];
  }
  //OS_ENTER_CRITICAL();
  if(ch==0)
  {
  	for(i=0;i<4;i++)
  	Ch1CurrLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(9,Ch1CurrLedBuf,4);

  	for(i=0;i<4;i++)
  	Ch2CurrLedBuf[i]=buf[i+4];
	if(g_stat.ALL_ON_FLG)
	{
	    SET_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
	}
	else
	{
	    SET_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
	}
    WriteAll_1621Byte_CH2(8,Ch2CurrLedBuf,4);
  }
  else
  {
  	for(i=0;i<4;i++)
  	Ch2CurrLedBuf[i]=buf[i];
	if(g_stat.ALL_ON_FLG)
	{
	    SET_BIT(buf[2], LED_BIT3); 
		CLEAR_BIT(buf[3], LED_BIT3); 
	}
	else
	{
	    SET_BIT(buf[3], LED_BIT3); 
		CLEAR_BIT(buf[2], LED_BIT3); 
	}
    WriteAll_1621Byte_CH2(8,buf,4);
  }
 // OS_EXIT_CRITICAL()
}
void HT1621_Disp_Cal_curret(unsigned char ch,float val)
{
	unsigned char i,buf[7];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
    if(val<1)
  {
	 if(val>0.9995)
	 {
		val=1;
	 }
  }
  floattostr1(buffer,val);
  if(val<1)
  {
   buf[0]=LCD_ADD_DOT[0]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD_DOT[j];

   j=buffer[5]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[7]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[8]%0x30;
   buf[7]=LCD_ADD[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD_DOT[j]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD_DOT[j];

   j=buffer[5]%0x30;
   buf[4]=LCD_ADD[j];
   j=buffer[6]%0x30;
   buf[5]=LCD_ADD[j];
   j=buffer[7]%0x30;
   buf[6]=LCD_ADD[j]; 
   j=buffer[8]%0x30;
   buf[7]=LCD_ADD[j];
  }
  //OS_ENTER_CRITICAL();
  if(ch==0)
  {
   	for(i=0;i<4;i++)
  	Ch1PowerLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(17,buf,4);

     	for(i=0;i<4;i++)
  	Ch2PowerLedBuf[i]=buf[i+4];
  WriteAll_1621Byte_CH2(16,buf,4);
  }
  else
  {
   	for(i=0;i<4;i++)
  	Ch2PowerLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH2(16,buf,4);
  }
 // OS_EXIT_CRITICAL()
}
void HT1621_Disp_power(unsigned char ch,float val)
{
	unsigned char i,buf[7];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
    if(val<100)
  {
	 if(val>99.995)
	 {
		val=100;
	 }
  }
  else if(val<10)
  {
 	 if(val>9.9995)
	 {
		val=10;
	 }
  }
  	
  floattostr(buffer,val);
  if(val>=100)
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD[j]; 
   j=buffer[1]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[2]%0x30;
   buf[2]=LCD_ADD_DOT[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD_DOT[j];
  	}	
 else if(val<10)
  {
   buf[0]=LCD_ADD[0]; 
   j=buffer[0]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[2]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[3]=LCD_ADD_DOT[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD[j]; 
   j=buffer[1]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD_DOT[j];
  }
 // OS_ENTER_CRITICAL();
  if(ch==0)
  {
   	for(i=0;i<4;i++)
  	Ch1PowerLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(17,buf,4);
  }
  else
  {
   	for(i=0;i<4;i++)
  	Ch2PowerLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH2(16,buf,4);
  }
  //OS_EXIT_CRITICAL()
}
void HT1621_Disp_volt(unsigned char ch,float val)
{
	unsigned char i,buf[7];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
   if(val<10)
   {
	if(val>9.996)
		val=10;
   }
  floattostr(buffer,val);
  if(val<10)
  {
   buf[0]=LCD_ADD[0]; 
   j=buffer[0]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[2]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[3]=LCD_ADD[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD[j]; 
   j=buffer[1]%0x30;
   buf[1]=LCD_ADD_DOT[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];
  }
  
  //OS_ENTER_CRITICAL();
  if(ch==0)
  {
  	for(i=0;i<4;i++)
  	Ch1VoltLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(0,buf,4);
  }
  else
  {
  	for(i=0;i<4;i++)
  	Ch2VoltLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH2(0,buf,4);
  }
 // OS_EXIT_CRITICAL()
}
void HT1621_Disp_curret(unsigned char ch,float val)
{
	unsigned char i,buf[7];
	unsigned char j;
	OS_CPU_SR  cpu_sr ;
	for(i=0;i<16;i++)
	{
        buffer[i]=0x0;
	}
  if(val<1)
  {
	 if(val>0.9995)
	 {
		val=1;
	 }
  }
  floattostr(buffer,val);
  if(val<1)
  {
   buf[0]=LCD_ADD_DOT[0]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];
  }
  else
  {
   j=buffer[0]%0x30;
   buf[0]=LCD_ADD_DOT[j]; 
   j=buffer[2]%0x30;
   buf[1]=LCD_ADD[j];
   j=buffer[3]%0x30;
   buf[2]=LCD_ADD[j];
   j=buffer[4]%0x30;
   buf[3]=LCD_ADD[j];
  }
  //OS_ENTER_CRITICAL();
  if(ch==0)
  {
  	for(i=0;i<4;i++)
  	Ch1CurrLedBuf[i]=buf[i];
    WriteAll_1621Byte_CH1(9,buf,4);
  }
  else
  {
  	for(i=0;i<4;i++)
  	Ch2CurrLedBuf[i]=buf[i];
	if(g_stat.ALL_ON_FLG)
	{
	    SET_BIT(buf[2], LED_BIT3); 
		CLEAR_BIT(buf[3], LED_BIT3); 
	}
	else
	{
	    SET_BIT(buf[3], LED_BIT3); 
		CLEAR_BIT(buf[2], LED_BIT3); 
	}
    WriteAll_1621Byte_CH2(8,buf,4);
  }
 // OS_EXIT_CRITICAL()
}

void HT1621_Disp_ON_FLG(unsigned char state)
{
	unsigned char regt;
	if(state)
	{
	    SET_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
	}
	else
	{
	    SET_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
	}
    regt=Ch2CurrLedBuf[2];
	WriteAll_1621_CH2(12,&regt,1); 
    regt=Ch2CurrLedBuf[3];
	WriteAll_1621_CH2(14,&regt,1); 	
	}
void DRAW_CLEAR_PARA(u8 clear)
{
	unsigned char regt;
	regt=0x02;
	if(clear)
	{
	    CLEAR_BIT(OTPLedBuf, LED_BIT0); 
		CLEAR_BIT(OTPLedBuf, LED_BIT2); 
	}
	else
	{
	    SET_BIT(OTPLedBuf, LED_BIT2); 
		CLEAR_BIT(OTPLedBuf, LED_BIT0); 
	}
	WriteAll_1621_CH1(27,&OTPLedBuf,1); 
}
void DRAW_CLEAR_OTP(u8 clear)
{
	unsigned char regt;
	regt=0x02;
	if(clear)
	{
	    SET_BIT(OTPLedBuf, LED_BIT3); 
	//	CLEAR_BIT(OTPLedBuf, LED_BIT2); 
	}
	else
	{
	 //   SET_BIT(OTPLedBuf, LED_BIT2); 
		CLEAR_BIT(OTPLedBuf, LED_BIT3); 
	}
	WriteAll_1621_CH1(27,&OTPLedBuf,1); 
}
void DRAW_CLEAR_SER(u8 clear)
{

	unsigned char regt;
	regt=0x02;
	if(clear)
	{
	    CLEAR_BIT(OTPLedBuf, LED_BIT0); 
		CLEAR_BIT(OTPLedBuf, LED_BIT2); 
	}
	else
	{
	    SET_BIT(OTPLedBuf, LED_BIT0); 
		CLEAR_BIT(OTPLedBuf, LED_BIT2); 
	}
	WriteAll_1621_CH1(27,&OTPLedBuf,1); 
}
void DRAW_LOCK_FLG(u8 clear)
{

	unsigned char regt;
	//regt=0x02;
	if(clear)
	{
        regt=0x0D;
	}
	else
	{
	    //SET_BIT(regt, LED_BIT0); 
		//CLEAR_BIT(regt, LED_BIT2); 
		regt=0x0b;
	}
	
	WriteAll_1621_CH1(29,&regt,1); 
}
void DRAW_BEEPER_FLG(u8 clear)
{

	unsigned char regt;
	regt=beepLedbuf;
	if(clear)
	{
	    CLEAR_BIT(regt, LED_BIT1); 
		SET_BIT(regt, LED_BIT2); 
	}
	else
	{
	    SET_BIT(regt, LED_BIT1); 
		CLEAR_BIT(regt, LED_BIT2); 
	}
	beepLedbuf=regt;
	WriteAll_1621_CH1(28,&regt,1); 
}
void DRAW_SET_FLG(u8 clear)
{

	unsigned char regt;
	regt=beepLedbuf;
	if(clear)
	{
	    SET_BIT(regt, LED_BIT0); 
		SET_BIT(regt, LED_BIT3); 
	}
	else
	{
	    CLEAR_BIT(regt, LED_BIT0); 
		CLEAR_BIT(regt, LED_BIT3); 
	}
	beepLedbuf=regt;
	WriteAll_1621_CH1(28,&regt,1); 
}
void draw_ovp_flg(u8 clear,u8 ch)
{

	unsigned char regt;
	if(ch==0)
		{
	regt=ch1OVPLedBuf;
	if(clear)
	{
	   // SET_BIT(regt, LED_BIT1); 
		SET_BIT(regt, LED_BIT0); 
	}
	else
	{
	   // CLEAR_BIT(regt, LED_BIT1); 
		CLEAR_BIT(regt, LED_BIT0); 
	}
	ch1OVPLedBuf=regt;
	WriteAll_1621_CH1(8,&regt,1); 

		}
	else
	{
		regt=ch2OVPLedBuf;
		if(clear)
		{
	    		//SET_BIT(regt, LED_BIT1); 
			SET_BIT(regt, LED_BIT0); 
		}
		else
		{
	    		//CLEAR_BIT(regt, LED_BIT1); 
			CLEAR_BIT(regt, LED_BIT0); 
		}
		ch2OVPLedBuf=regt;
		WriteAll_1621_CH1(30,&regt,1); 
	}
}
void draw_ocp_flg(u8 clear,u8 ch)
{

	unsigned char regt;
	if(ch==0)
		{
	regt=ch1OVPLedBuf;
	if(clear)
	{
	    SET_BIT(regt, LED_BIT1); 
		//SET_BIT(regt, LED_BIT0); 
	}
	else
	{
	    CLEAR_BIT(regt, LED_BIT1); 
		//CLEAR_BIT(regt, LED_BIT0); 
	}
	ch1OVPLedBuf=regt;
	WriteAll_1621_CH1(8,&regt,1); 

		}
	else
	{
		regt=ch2OVPLedBuf;
		if(clear)
		{
	    		SET_BIT(regt, LED_BIT1); 
			//SET_BIT(regt, LED_BIT0); 
		}
		else
		{
	    		CLEAR_BIT(regt, LED_BIT1); 
			//CLEAR_BIT(regt, LED_BIT0); 
		}
		ch2OVPLedBuf=regt;
		WriteAll_1621_CH1(30,&regt,1); 
	}
}
void DRAW_OVP_OCP_FLG(u8 clear)
{

	unsigned char regt;
	regt=ch1OVPLedBuf;
	if(clear)
	{
	    SET_BIT(regt, LED_BIT1); 
		SET_BIT(regt, LED_BIT0); 
	}
	else
	{
	    CLEAR_BIT(regt, LED_BIT1); 
		CLEAR_BIT(regt, LED_BIT0); 
	}
	ch1OVPLedBuf=regt;
	WriteAll_1621_CH1(8,&regt,1); 
	regt=ch2OVPLedBuf;
	if(clear)
	{
	    SET_BIT(regt, LED_BIT1); 
		SET_BIT(regt, LED_BIT0); 
	}
	else
	{
	    CLEAR_BIT(regt, LED_BIT1); 
		CLEAR_BIT(regt, LED_BIT0); 
	}
	ch2OVPLedBuf=regt;
	WriteAll_1621_CH1(30,&regt,1); 
}
extern u8 ch3_ChannelSel;
void HT1621_set_bit(unsigned char addr,unsigned char ch,unsigned char bit)
{
  unsigned char regt;
   if(ch==0)
   	{
 regt=ReadHT1621Byte(addr,4);
   	}
   else
   {
   regt=ReadHT1621ByteCH2(addr,4);
   }
 regt |=(1<<bit);
 if(ch==0)
 {
	WriteAll_1621_CH1(addr,&regt,1);
 }
 else
 {
	WriteAll_1621_CH2(addr,&regt,1);
 }
}
void HT1621_clear_bit(unsigned char addr,unsigned char ch,unsigned char bit)
{
  unsigned char regt,a;
  a=0;
	if(ch==0)
	 {
  regt=ReadHT1621Byte(addr,4);
	 }
	else
	{
	regt=ReadHT1621ByteCH2(addr,4);
	}

 a=(1<<bit);
 a=(~a);
 regt &=a;
 if(ch==0)
 {
	WriteAll_1621_CH1(addr,&regt,1);
 }
 else
 {
	WriteAll_1621_CH2(addr,&regt,1);
 }

}

void HT1621_Test(void)
{
	float tes,curtest;
    unsigned char i,regt,a[10],ledreg;
	tes=10;
	curtest=3;
	ch1OVPLedBuf=0x00;
	ch2OVPLedBuf=0x00;
	beepLedbuf=0x04;
	for(i=0;i<32;i++)
	{
        buffer[i]=0x0;
	}
	Ht1621_CS1(1); 
	Ht1621_CS0(1);
    //buffer[0]=0x21;buffer[1]=0x43;buffer[2]=0x65;buffer[3]=0x87;a[4]=0xa9;
   // _pac=0; //将端口设为输出
    SendCmd(BIAS); //设置偏压和占空比
    SendCmd(SYSEN);  //打开系统振荡器
    SendCmd(LCDON);  //打开 LCD 偏压发生器

	
    WriteAll_1621Byte_CH1(0,buffer,16); 

     //buffer[0]=0x01;
	//WriteAll_1621_CH1(25,buffer,1); 

	
	regt=0x09;
	WriteAll_1621_CH1(26,&regt,1); 

	regt=0x07;
	WriteAll_1621_CH1(31,&regt,1); 	
	regt=0x0b;
	WriteAll_1621_CH1(29,&regt,1); 
	regt=0x02;
	WriteAll_1621_CH1(27,&regt,1); 	

	WriteAll_1621_CH1(8,&ch1OVPLedBuf,1);
	WriteAll_1621_CH1(30,&ch1OVPLedBuf,1); 
	//WriteAll_1621_CH1(28,&beepLedbuf,1); 
	//Select_CH3(ch3_ChannelSel);
	CH3_value_set(CHAN3.volt_set);
    //Write_1621(0x09,0x01); //在地址 0x09 处写入数据 0x01
   // while(1)
	//for(i=0;i<32;i++)	
	HT1621_Disp_volt(0,CHAN1.volt_set);
	HT1621_Disp_curret(0,CHAN1.curr_set);
	CHAN1.Pow_set=CHAN1.volt_set*CHAN1.curr_set;
	HT1621_Disp_power(0,CHAN1.Pow_set);
   // regt=ReadHT1621Byte(0,8);

	for(i=0;i<32;i++)
	{
       // buffer[i]=LCD_ADD[9];
	}	
    {

    }
    
    Ht1621_CS0(1); 
    SendCmd_CH2(BIAS); //设置偏压和占空比
    SendCmd_CH2(SYSEN);  //打开系统振荡器
    SendCmd_CH2(LCDON);  //打开 LCD 偏压发生器
	for(i=0;i<32;i++)
	{
        buffer[i]=0x0;
	}
	WriteAll_1621Byte_CH2(0,buffer,16); 
    HT1621_Disp_volt(1,CHAN2.volt_set);
	HT1621_Disp_curret(1,CHAN2.curr_set);
	CHAN2.Pow_set=CHAN2.volt_set*CHAN2.curr_set;
	HT1621_Disp_power(1,CHAN2.Pow_set);

	HT1621_Disp_ON_FLG(0);
	regt=0x04;
	WriteAll_1621_CH2(26,&regt,1); 

		//regt=0x0a;
	//  WriteAll_1621Byte_CH2(30,&regt,1);
	  //regt=0;
	  //regt=ReadHT1621Byte(26,4) ;
	  //regt &=0xf0;
		//regt=0;
	 // regt=ReadHT1621ByteCH2(30,4) ;
	for(i=0;i<32;i++)
	{
       // buffer[i]=LCD_ADD_DOT[8];
	}	
	//for(i=0;i<32;i++)	
    {
       // WriteAll_1621Byte_CH2(0,buffer,16);  //在起始地址为 0 处连续写入 5 个字节数据
        DELAY_MS(1);
    }
	for(i=0;i<32;i++)
	{
        buffer[i]=LCD_ADD[0];;
	}
	
    if(BeeperFlg)
    {
        LED_BEEP(1);
	}
    else
    {
	LED_BEEP(0);
	}
		ledreg=0x08;
	WriteAll_1621_CH1(25,&ledreg,1); 
	DRAW_BEEPER_FLG(BeeperFlg);
}
void ch3_test_disp(void)
{
	unsigned char i,buf[5];
	for(i=0;i<5;i++)
	{
        buf[i]=0xff;
	}
   // HT1621_Disp_Save_count(3);

	HT1621_Disp_CH3_volt(CHAN3.volt_set);
	//WriteAll_1621Byte_CH2(24,buf,1);
	

}
void PowerOn_Test(void)
{
    unsigned char i,regt,a[10];

	for(i=0;i<32;i++)
	{
        buffer[i]=0xff;
	}
	Ht1621_CS1(1); 
	Ht1621_CS0(1);
    //buffer[0]=0x21;buffer[1]=0x43;buffer[2]=0x65;buffer[3]=0x87;a[4]=0xa9;
   // _pac=0; //将端口设为输出
    SendCmd(BIAS); //设置偏压和占空比
    SendCmd(SYSEN);  //打开系统振荡器
    SendCmd(LCDON);  //打开 LCD 偏压发生器
	WriteAll_1621Byte_CH1(0,buffer,16); 
    Ht1621_CS0(1); 
    SendCmd_CH2(BIAS); //设置偏压和占空比
    SendCmd_CH2(SYSEN);  //打开系统振荡器
    SendCmd_CH2(LCDON);  //打开 LCD 偏压发生器
	
	WriteAll_1621Byte_CH2(0,buffer,16);

	//WriteAll_1621Byte_CH2(0,0x,4);
	LED_CH1_CV_OFF ;
	LED_CH2_CV_OFF ;
	LED_SER_CH1(1);
	LED_PARA_CH2(1);
	LED_CH3_1V8(1);
	LED_CH3_2V5(1);
	LED_CH3_3V3(1);
	LED_CH3_3V3(1);
	LED_CH3_5V0(1);
	LED_ON_OFF(1);
	LED_OVP(1);
	LED_OCP(1);
	LED_BEEP(1);
	LED1_M1(1);
	LED1_M2(1);
	LED1_M3(1);
	LED1_M4(1);
	LED1_M5(1);
	LED_CH3(1);
	LED_SETVIEW(1);
	LED_LOCK(1);
	DELAY_MS1(100);
	//BeeperFlg=1;
	BEEP_RING( 50 );
	DELAY_MS1(10);
	LED_LOCK(0);
}



