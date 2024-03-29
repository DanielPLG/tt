/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			LCDConf.c
** Descriptions:		LCD Configuration
**						
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2010-11-9
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

#ifndef LCDCONF_H
#define LCDCONF_H


#define LCD_XSIZE          (480)
#define LCD_YSIZE          (272)
#define LCD_CONTROLLER     (9320)
#define LCD_BITSPERPIXEL   (16)
#define LCD_FIXEDPALETTE   (565)
#define LCD_SWAP_RB        (1)
//#define LCD_SWAP_XY        (1)
#define LCD_INIT_CONTROLLER()  ili9320_Initializtion();
#endif /* LCDCONF_H */

#define Bank1_LCD_D    ((uint32_t)0x60020000)    //disp Data ram
#define Bank1_LCD_C    ((uint32_t)0x60000000)	 //disp Reg ram
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

