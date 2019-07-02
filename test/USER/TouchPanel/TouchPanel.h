/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.h
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

#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_
#define EPR_CS(x)    	x ? GPIO_SetBits(GPIOB,GPIO_Pin_7): GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define TP_CS(x)    	x ? GPIO_SetBits(GPIOB,GPIO_Pin_12): GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define HC595_STR(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_6): GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define ch2HC595_STR(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_1): GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define DA7562_CLK(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_13): GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define DA7562_CS(x)	x ? GPIO_SetBits(GPIOD,GPIO_Pin_8): GPIO_ResetBits(GPIOD,GPIO_Pin_8)
#define DA7562_LDAC(x)	x ? GPIO_SetBits(GPIOD,GPIO_Pin_11): GPIO_ResetBits(GPIOD,GPIO_Pin_11)
#define DA7562_DIN(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_15): GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define AD7792_CLK(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_13): GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define AD7792_DOUT      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)   
#define AD7792_DIN(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_15): GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define ch2TP_CS(x)    	x ? GPIO_SetBits(GPIOA,GPIO_Pin_0): GPIO_ResetBits(GPIOA,GPIO_Pin_0)

#define ch2DA7562_CLK(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_1): GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define ch2DA7562_CS(x)	x ? GPIO_SetBits(GPIOE,GPIO_Pin_5): GPIO_ResetBits(GPIOE,GPIO_Pin_5)
#define ch2DA7562_LDAC(x)	x ? GPIO_SetBits(GPIOE,GPIO_Pin_0): GPIO_ResetBits(GPIOE,GPIO_Pin_0)
#define ch2DA7562_DIN(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_3): GPIO_ResetBits(GPIOA,GPIO_Pin_3)

#define ch2AD7792_CLK(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_1): GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define ch2AD7792_DOUT    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) 
#define ch2AD7792_DIN(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_3): GPIO_ResetBits(GPIOA,GPIO_Pin_3)

#define ADC122S021_CLK(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_13): GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define ADC122S021_DOUT      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)   
#define ADC122S021_DIN(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_15): GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define ch2ADC122_CLK(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_1): GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define ch2ADC122_DOUT    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) 
#define ch2ADC122_DIN(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_7): GPIO_ResetBits(GPIOA,GPIO_Pin_7)

#define ch3AD_CS(x)    	x ? GPIO_SetBits(GPIOC,GPIO_Pin_5): GPIO_ResetBits(GPIOC,GPIO_Pin_5)
#define ch3DA_CS(x)    	x ? GPIO_SetBits(GPIOB,GPIO_Pin_0): GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define ADS7040_CLK(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_5): GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define ADS7040_DOUT      GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)   
#define ADS7040_DIN(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_7): GPIO_ResetBits(GPIOA,GPIO_Pin_7)

#define CH4_ON_DELAY(x)	x ? GPIO_SetBits(GPIOA,GPIO_Pin_4): GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define CH3_ON_DELAY(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_11): GPIO_ResetBits(GPIOB,GPIO_Pin_11)


#define RDATA_REG   0x58
#define WDATA_REG   0x18
#define RMODE_REG   0x48
#define WMODE_REG   0x08
#define RCONFIG_REG   0x50
#define WCONFIG_REG   0x10
#define ROFFSET_REG   0x70
#define WOFFSET_REG   0x30
#define RFULL_REG   0x78
#define WFULL_REG   0x38

#define BIAS 0x29
#define SYSEN 0x01
#define LCDOFF 0x02
#define LCDON 0x03
#define cs _pa0
#define wr _pa2
#define dat _pa3

#define Ht1621_CS0(x)	x ? GPIO_SetBits(GPIOC,GPIO_Pin_0): GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define Ht1621_CS1(x)	x ? GPIO_SetBits(GPIOC,GPIO_Pin_1): GPIO_ResetBits(GPIOC,GPIO_Pin_1)

#define Ht1621_WR(x)	x ? GPIO_SetBits(GPIOC,GPIO_Pin_3): GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define Ht1621_RD(x)	x ? GPIO_SetBits(GPIOC,GPIO_Pin_2): GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define Ht1621_Dat(x)	x ? GPIO_SetBits(GPIOB,GPIO_Pin_2): GPIO_ResetBits(GPIOB,GPIO_Pin_2)

#define Ht1621_DatIn    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) 

//#define READ_SDA	PFn(6)  //输入SDA
//IO方向设置
#define Ht1621_Dat_IN() 		{GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=0X00000800;GPIOB->ODR|=1<<2;}	//PB9输入模式
#define Ht1621_Dat_OUT()  	{GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=0x0700;} //PB9输出模式

//#define Ht1621_Dat_IN() 		{GPIOA->MODER&=~(3<<(0*2));GPIOA->MODER|=0<<0*2;}	//PB9输入模式
//#define Ht1621_Dat_OUT()  	{GPIOA->MODER&=~(3<<(0*2));GPIOA->MODER|=1<<0*2;} //PB9输出模式
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT 
{
   uint16_t x;
   uint16_t y;
}Coordinate;


typedef struct Matrix 
{						
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
} Matrix ;

/* Private variables ---------------------------------------------------------*/
extern Coordinate ScreenSample[3];
extern Coordinate DisplaySample[3];
extern Matrix matrix ;
extern Coordinate  display ;

/* Private define ------------------------------------------------------------*/
/* AD通道选择命令字和工作寄存器 */
#define	CHX 	0x90 	/* 通道Y+的选择控制字 */	
#define	CHY 	0xd0	/* 通道X+的选择控制字 */


					
#define TP_INT_IN   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)

/* Private function prototypes -----------------------------------------------*/				
void TP_Init(void);	
Coordinate *Read_Ads7846(void);
void TouchPanel_Calibrate(void);
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos);
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr);
FunctionalState getDisplayPoint(Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr );
void SendBit_1621(unsigned char data,unsigned char cnt);
void SendDataBit_1621(unsigned char data,unsigned char cnt);
void SendCmd(unsigned char command);
void Write_1621(unsigned char addr,unsigned char data);
void WriteAll_1621(unsigned char addr,unsigned char *p,unsigned char cnt);
void SendBit_1621_CH2(unsigned char data,unsigned char cnt);
void SendDataBit_1621_CH2(unsigned char data,unsigned char cnt);
void SendCmd_CH2(unsigned char command);
void Write_1621_CH2(unsigned char addr,unsigned char data);
void WriteAll_1621_CH2(unsigned char addr,unsigned char *p,unsigned char cnt);
void HT1621_Disp_volt(unsigned char ch,float val);
void HT1621_Disp_power(unsigned char ch,float val);
void HT1621_Disp_curret(unsigned char ch,float val);
void HT1621_Disp_Cal_curret(unsigned char ch,float val);
void HT1621_Disp_CH3_volt(double val);
u16 CH3_ADS7040_Write_Read(u16 byte);
void CH3_set_Dac(u16 byte);
void HT1621_set_bit(unsigned char addr,unsigned char ch,unsigned char bit);
void HT1621_clear_bit(unsigned char addr,unsigned char ch,unsigned char bit);

void floattostr2(char*buf,double tmp);

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


