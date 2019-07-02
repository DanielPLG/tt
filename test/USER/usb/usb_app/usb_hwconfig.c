#include "stm32f10x.h"
#include "usb_prop.h"
#include "Main.h"
#include "stm32f10x_usart.h"
#include <stdio.h>
#include <string.h>
/*#define ENDP1_TXADDR        (0xC0)
#define ENDP2_TXADDR        (0x100)
#define ENDP3_RXADDR        (0x110)*/
#define ENDP0_RXADDR        (0x40)
#define ENDP0_TXADDR        (0x80)
#define ENDP1_RXADDR        (0xC0)
#define ENDP2_TXADDR        (0x110)
#define ENDP0   ((u8)0)
#define ENDP1   ((u8)1)
#define ENDP2   ((u8)2)
#define ENDP3   ((u8)3)
#define VIRTUAL_COM_PORT_DATA_SIZE              64
//void floattostr(char*buf,float tmp);
typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;
USART_InitTypeDef USART_InitStructure;
CMD SCPI;
extern LINE_CODING linecoding;
void USART_Config_Default(void)
{
  /* USART1 default configuration */
  /* USART1 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Parity Odd
        - Hardware flow control desabled
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 /* USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
*/
  /* Configure the USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable the USART Receive interrupt */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}
/*******************************************************************************
* Function Name  :  UART0_Config.
* Description    :  Configure the UART 0 according to the linecoding structure.
* Input          :  None.
* Return         :  Configuration status
                    TRUE : configuration done with success
                    FALSE : configuration aborted.
*******************************************************************************/
bool USART_Config(void)
{
  /* set the bit rate*/
  if ((linecoding.bitrate > 115200) || (linecoding.bitrate < 1200) )
  {
    USART_Config_Default();
    return (FALSE);
  }

  /* set the Stop bit*/
  switch (linecoding.format)
  {
    case 0:
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      break;
    case 1:
      USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
      break;
    case 2:
      USART_InitStructure.USART_StopBits = USART_StopBits_2;
      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  /* set the parity bit*/
  switch (linecoding.paritytype)
  {
    case 0:
      USART_InitStructure.USART_Parity = USART_Parity_No;
      break;
    case 1:
      USART_InitStructure.USART_Parity = USART_Parity_Even;
      break;
    case 2:
      USART_InitStructure.USART_Parity = USART_Parity_Odd;
      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  /*set the data type : only 8bits and 9bits is supported */
  switch (linecoding.datatype)
  {
    case 0x07:
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      break;
    case 0x08:
      USART_InitStructure.USART_WordLength = USART_WordLength_9b;
      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  USART_InitStructure.USART_BaudRate = linecoding.bitrate;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);
  return (TRUE);
}
//====================================================================
// USB发送数据
// input:  buf 发送的数组
//         num 发送数据的字节数
// output: 发送数据的字节数
//====================================================================
extern u8 TxBusy;
u8 USB_SEND(u8 *buf,u8 num)//num 需小于64
{  
 UserToPMABufferCopy(buf, ENDP2_TXADDR, num);
 SetEPTxCount(ENDP2, num);
 SetEPTxValid(ENDP2);
  TxBusy=0	;
 //toupper(10);
 return num;
}
u8 USB_CONTROL_SEND(u8 *buf,u8 num)//num 需小于64
{  
 UserToPMABufferCopy(buf, ENDP0_TXADDR, num);
 SetEPTxCount(ENDP0, num);
 SetEPTxValid(ENDP0);
 //toupper(10);
 return num;
}
u8 USB_SEND_String(char *buf)//num 需小于64
{
 char Result[10];
 u8 num;
 num=10;
 sprintf(Result,buf);  
 UserToPMABufferCopy(Result, ENDP2_TXADDR, num);
 SetEPTxCount(ENDP2, num);
 SetEPTxValid(ENDP2);
 return num;
}
u8 USB_SEND_FLOAT(float tmp)//num 需小于64
{
 char Result[10];
 u8 num;
 num=10;
 
 sprintf(Result,"%.3f",tmp);  
 printf(Result);
 UserToPMABufferCopy(Result, ENDP2_TXADDR, num);
 SetEPTxCount(ENDP2, num);
 SetEPTxValid(ENDP2);
 return num;
}
/*void floattostr(char*buf,float tmp)
{

sprintf(buf,"%.3f",tmp); 

}*/
u8 USB_putchar(u8 c)//num 需小于64
{  
 u8 buf[1];
  buf[0]=c;
 UserToPMABufferCopy(buf, ENDP2_TXADDR, 1);
 SetEPTxCount(ENDP2, 1);
 SetEPTxValid(ENDP2);
 return c;
}
 //====================================================================
// 读取USB寄存器里的数据
// 有数据传输时，会有中断响应，count_out!=0 说明有效数据在寄存器里
// 便读取buffer的数据
//====================================================================
u8 usbtmc[17]={0x02,0x05,0xfa,0x00,0x05,0x00,0x00,0x00,
 0x01,0x00,0x00,0x00,0x52,0x69,0x67,0x6f,0x6c,
};

u8 USB_RECV_BLOCK(u8 *buf)
{
  u8 num;
  u8 i;
  u8 tx_buf[2]; 
  extern u32 count_out;
  extern u8 buffer_out[VIRTUAL_COM_PORT_DATA_SIZE];
  while(1)
   {
    if(count_out !=0 )
    {
      memcpy(buf,buffer_out,40);
	 /* if(SCPI_decode(buf))
	  USB_SEND_String(&SCPI.COMMD[2][0]); */
	  for(i=0;i<30;i++)
	  {
	   buffer_out[i]=0x00;
  	  }
      num = count_out;
	  
      count_out = 0;
//	DELAY_MS(1000);
      if(buf[0]==0x02)
	  {
	 usbtmc[1]=buf[1];
	 usbtmc[2]=buf[2];
	 USB_SEND(usbtmc,17);

	  }
//	   DELAY_MS(1000); 
	  if(strcmp("volt",buf)==0)
	  {
	   USB_SEND_String("abcdef");
	  }
	 /*   if(buf[0]==0x12)
	  {	
	
	  tx_buf[0]=0x05;
	  tx_buf[1]=0x06;

	  USB_SEND_String("*CL S");
	 // USB_SEND_FLOAT(123.340);
	 // printf("aa");
	 //USB_SEND(tx_buf,2);
	  } */     
    } 
   }
   return num;
}

