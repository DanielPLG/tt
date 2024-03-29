/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : All processing related to Virtual COM Port Demo (Endpoint 0)
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usb_prop_H
#define __usb_prop_H
#include "usb_core.h"
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
void Virtual_Com_Port_init(void);
void Virtual_Com_Port_Reset(void);
void Virtual_Com_Port_Status_In (void);
void Virtual_Com_Port_Status_Out (void);
RESULT Virtual_Com_Port_Data_Setup(u8);
RESULT Virtual_Com_Port_NoData_Setup(u8);
RESULT Virtual_Com_Port_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *Virtual_Com_Port_GetDeviceDescriptor(u16 );
u8 *Virtual_Com_Port_GetConfigDescriptor(u16);
u8 *Virtual_Com_Port_GetStringDescriptor(u16);

u8 *Virtual_Com_Port_GetLineCoding(u16 Length);
u8 *Virtual_Com_Port_SetLineCoding(u16 Length);

/* Exported define -----------------------------------------------------------*/
typedef struct
{
  u8 bitrate;
  u8 format;
  u8 paritytype;
  u8 datatype;
  u8 data0;
  u8 data1;
  u8 data2;
  u8 data3;
  u8 data4;
  u8 data5;
  u8 data6;
  u8 data7;
  u8 data8;
  u8 data9;
  u8 data10;
  u8 data11;
  u8 data12;
  u8 data13;
  u8 data14;
  u8 data15;
  u8 data16;
  u8 data17;
  u8 data18;
  u8 data19;

}LINE_CODING;

#define Virtual_Com_Port_GetConfiguration          NOP_Process
#define Virtual_Com_Port_SetConfiguration          NOP_Process
#define Virtual_Com_Port_GetInterface              NOP_Process
#define Virtual_Com_Port_SetInterface              NOP_Process
#define Virtual_Com_Port_GetStatus                 NOP_Process
#define Virtual_Com_Port_ClearFeature              NOP_Process
#define Virtual_Com_Port_SetEndPointFeature        NOP_Process
#define Virtual_Com_Port_SetDeviceFeature          NOP_Process
#define Virtual_Com_Port_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0xa0
#define GET_LINE_CODING             0x07
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23
/* USBTMC class-specific bRequest values */
#define  USBTMC_INITIATE_ABORT_BULK_OUT      1
#define  USBTMC_CHECK_ABORT_BULK_OUT_STATUS  2
#define  USBTMC_INITIATE_ABORT_BULK_IN       3
#define  USBTMC_CHECK_ABORT_BULK_IN_STATUS   4
#define  USBTMC_INITIATE_CLEAR               5
#define  USBTMC_CHECK_CLEAR_STATUS           6
#define  USBTMC_GET_CAPABILITIES             7
#define  USBTMC_INDICATOR_PULSE             64
#define  USBTMC_READ_STATUS_BYTE           128
#define  USBTMC_REN_CONTROL                160
#define  USBTMC_GO_TO_LOCAL                161
#define  USBTMC_LOCAL_LOCKOUT              162
#endif /* __usb_prop_H */
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

