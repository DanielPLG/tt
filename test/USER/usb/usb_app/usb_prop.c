/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : All processing related to Virtual Com Port Demo
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"

#include "usb_desc.h"
#include "usb_pwr.h"  
#include "usb_hwconfig.h"
u8 *response(u16 Length);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 Request = 0;
u8 ren=0x01;
u8 stat[2]={0x80,0x00};
u8 answer[10];
u8 answer_num;
LINE_CODING linecoding =
{
	0x01,
	0x00,
	0x00,   
	0x01, 
	0x00,   /* stop bits-1*/
	0x01,
	0x00,   /* parity - none*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,															
	0x00,
	0x01,
	0x07,
	0x0f,    /* no. of bits 8*/
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    Virtual_Com_Port_init,
    Virtual_Com_Port_Reset,
    Virtual_Com_Port_Status_In,
    Virtual_Com_Port_Status_Out,
    Virtual_Com_Port_Data_Setup,
    Virtual_Com_Port_NoData_Setup,
    Virtual_Com_Port_Get_Interface_Setting,
    Virtual_Com_Port_GetDeviceDescriptor,
    Virtual_Com_Port_GetConfigDescriptor,
    Virtual_Com_Port_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Virtual_Com_Port_GetConfiguration,
    Virtual_Com_Port_SetConfiguration,
    Virtual_Com_Port_GetInterface,
    Virtual_Com_Port_SetInterface,
    Virtual_Com_Port_GetStatus,
    Virtual_Com_Port_ClearFeature,
    Virtual_Com_Port_SetEndPointFeature,
    Virtual_Com_Port_SetDeviceFeature,
    Virtual_Com_Port_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (u8*)Virtual_Com_Port_DeviceDescriptor,
    VIRTUAL_COM_PORT_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (u8*)Virtual_Com_Port_ConfigDescriptor,
    VIRTUAL_COM_PORT_SIZ_CONFIG_DESC
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(u8*)Virtual_Com_Port_StringLangID, VIRTUAL_COM_PORT_SIZ_STRING_LANGID},
    {(u8*)Virtual_Com_Port_StringVendor, VIRTUAL_COM_PORT_SIZ_STRING_VENDOR},
    {(u8*)Virtual_Com_Port_StringProduct, VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT},
    {(u8*)Virtual_Com_Port_StringSerial, VIRTUAL_COM_PORT_SIZ_STRING_SERIAL}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Virtual_Com_Port_init.
* Description    : Virtual COM Port Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_init(void)
{

  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  /* clear pending interrupts */
  _SetISTR(0);
  wInterrupt_Mask = IMR_MSK;
  /* set interrupts mask */
  _SetCNTR(wInterrupt_Mask);
  pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];
  /* configure the USART 1 to the default settings */
  //USART_Config_Default();
  /* Wait until device is configured */	  /*
  while (pInformation->Current_Configuration == 0)
  {
    NOP_Process();
  }
  bDeviceState = CONFIGURED;*/
}
	
#define KEY_CANCLE 0xf3	   
unsigned char Virtual_Com_Port_init_return(void)
{

  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  /* clear pending interrupts */
  _SetISTR(0);
  wInterrupt_Mask = IMR_MSK;
  /* set interrupts mask */
  _SetCNTR(wInterrupt_Mask);
  pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];
  /* configure the USART 1 to the default settings */
  //USART_Config_Default();
  /* Wait until device is configured  */
  while (pInformation->Current_Configuration == 0)
  {
   // if(KEY_CANCLE == Scankey())
      return 0;
  }								
  bDeviceState = CONFIGURED;
  return 1;
}
/*******************************************************************************
* Function Name  : Virtual_Com_Port_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Reset(void)
{
  /* Set Virtual_Com_Port DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Set Virtual_Com_Port DEVICE with the default Interface*/
  pInformation->Current_Interface = 0;
  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 2 */
  /*SetEPType(ENDP2, EP_INTERRUPT);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPRxStatus(ENDP2, EP_RX_DIS);
  SetEPTxStatus(ENDP2, EP_TX_NAK);
	*/
  /* Initialize Endpoint 3 */
  SetEPType(ENDP1, EP_BULK);
  SetEPRxAddr(ENDP1, ENDP1_RXADDR);
  SetEPRxCount(ENDP1, VIRTUAL_COM_PORT_DATA_SIZE);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPTxStatus(ENDP1, EP_TX_DIS);
   /* Initialize Endpoint 1 */
  SetEPType(ENDP2, EP_BULK);
  SetEPTxAddr(ENDP2, ENDP2_TXADDR);
  SetEPTxStatus(ENDP2, EP_TX_NAK);
  SetEPRxStatus(ENDP2, EP_RX_DIS);
 //printf("Virtual_Com_Port_Reset\r\n");
  /* Set this device to response on default address */
  SetDeviceAddress(0);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Status_In(void)
{
  if (Request == SET_LINE_CODING)
  {
    USART_Config();
    Request = 0;
  }
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Status_Out (void)
{
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Virtual_Com_Port_Data_Setup(u8 RequestNo)
{
  u8    *(*CopyRoutine)(u16);

  CopyRoutine = NULL;
  setup_req_handler();
//  PMAToUserBufferCopy((u8 *) &(ControlData.DeviceRequest), GetEPRxAddr(ENDP0), 8);
  if (RequestNo == GET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Virtual_Com_Port_GetLineCoding;
    }
  }
  else if (RequestNo == SET_LINE_CODING)
  {
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
      CopyRoutine = Virtual_Com_Port_SetLineCoding;
    }
  //  Request = SET_LINE_CODING;
  }
 else if (RequestNo == USBTMC_INITIATE_ABORT_BULK_OUT)
 {
 
  UsbTMCInitiateAbortBulkOut();
  answer_num=2;
  CopyRoutine = response;
 }
 else if (RequestNo ==USBTMC_CHECK_ABORT_BULK_OUT_STATUS)
 {
  UsbTMCCheckAbortBulkOutStatus();
   answer_num=8;
  CopyRoutine = response;
 }
 else if (RequestNo == USBTMC_INITIATE_ABORT_BULK_IN)
 {
//  printf("USBTMC_INITIATE_ABORT_BULK_IN\r\n");
  if (Type_Recipient == (CLASS_REQUEST | ENDPOINT_RECIPIENT))
 {	  
  UsbTMCInitiateAbortBulkIn();  
  answer_num=2;
  CopyRoutine = response;
 // usbtmc_initialise();
 }
 
 }
 else if (RequestNo == USBTMC_CHECK_ABORT_BULK_IN_STATUS)
 {
 UsbTMCCheckAbortBulkInStatus();
  answer_num=8;
  CopyRoutine = response;
 }
 else if (RequestNo == USBTMC_INITIATE_CLEAR)
 {
  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
  UsbTMCInitiateClear();
  answer_num=1;
  CopyRoutine = response;
  }
 }
 else if (RequestNo == USBTMC_CHECK_CLEAR_STATUS)
 {
  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
  UsbTMCCheckClearStatus();
  answer_num=2;
  CopyRoutine = response;
  }
 }
 else if (RequestNo == USBTMC_INDICATOR_PULSE)
 {
 }
 else if (RequestNo == USBTMC_READ_STATUS_BYTE)
 {
 
  UsbTMC488ReadStatusByte();
  answer_num=3;
  CopyRoutine = response;
 }


  if (CopyRoutine == NULL)
  {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Virtual_Com_Port_NoData_Setup(u8 RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
    if (RequestNo == SET_COMM_FEATURE)
    {
      return USB_SUCCESS;
    }
    else if (RequestNo == SET_CONTROL_LINE_STATE)
    {
      return USB_SUCCESS;
    }
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *Virtual_Com_Port_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor );
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *Virtual_Com_Port_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor );
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *Virtual_Com_Port_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
  return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0] );
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : u8: Interface : interface number.
* Input2         : u8: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Virtual_Com_Port_Get_Interface_Setting(u8 Interface, u8 AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Inecoding structure base address.
*******************************************************************************/
u8 *Virtual_Com_Port_GetLineCoding(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(u8 *)&linecoding;
}
u8 *response(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = answer_num;
    return NULL;
  }
  return(u8 *)&answer;
}
/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
u8 *Virtual_Com_Port_SetLineCoding(u16 Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(ren);
    return NULL;
  }
  return(u8 *)&ren;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

