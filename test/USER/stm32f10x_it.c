/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include "Main.h"
#include "stm32f10x_it.h"
#include "stm32f10x_exti.h"
#define RTC_IT_SEC           ((u16)0x0001)  /* Second interrupt */
#if ( OS_VIEW_MODULE == DEF_ENABLED )
extern	void OSView_RxTxISRHandler(void);
#endif
extern int LcdBackCount;
extern u8 OTP_beeper_flg,exit_trig_stat,OTPMode_flg, restore_disp_flg,OTP_restore_flg;
int LED_COUNT;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
u8 exti_trig=0;
int ext_counter=0;
/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
 extern u8 timer_start;



int beep_cnt=0;
u8 beeper_flg;
void SysTick_Handler(void)
{
 
	CPU_SR         cpu_sr;

  	if(beep_on)
	{
	beep_cnt++;
	if(beep_cnt>80)
	{
	beep_cnt=0;
	beep_on=0;
	} 
	beeper_flg=~beeper_flg;
	if(beeper_flg)
	 {
	 BEEP_ON;
	 }
	 else
	 {
	 BEEP_OFF;
	 }
	 }

	OS_ENTER_CRITICAL();                    						   /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
//	timer_show();
    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int tst;
void USB_LP_CAN1_RX0_IRQHandler(void)
{	
 /* GUI_GotoXY(10,0);
   tst++;
  GUI_DispDecSpace(tst,2);	 */	
  USB_Istr();
 
}
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u32 TimeDisplay;
int ht1621_count=0;

int ovp_count=0;
u8  ovp_flg=0;
u8  ht1621_flg=0;
u8  ovp_flash_flg=0;
extern int OTP_count;
extern u8 fine_set_flg,SetViewStart,SetViewFlg;
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);	
		
		ht1621_count++;
		if(ht1621_count>2)
		{
			ht1621_count=0;
			ht1621_flg=1;
		}
		if(ovp_flash_flg)
		{
		ovp_count++;
		if(ovp_count>3)
		{
			ovp_count=0;
			ovp_flg=1;
		}	
	}		
   ext_counter++;
		if(ext_counter==2)
		{
			
			exti_trig=0;
			ext_counter=0;
		}
	if( fine_set_flg)
	{
	TIMER_CH1.sec_count++;	
	if(TIMER_CH1.sec_count>4)
	{
	 fine_set_flg=0;
	  restore_disp_flg=1;
	 TIMER_CH1.sec_count=0;
	}

	}
	      	 if( OTP_restore_flg==1)
		OTP_count++;
      if(OTPMode_flg)
      	{

	TIMER_CH2.sec_count++;	
	if(TIMER_CH2.sec_count>4)
	{
	 //fine_set_flg=0;
		OTP_beeper_flg=1;
	 TIMER_CH2.sec_count=0;
	}
	  }
	if(SetViewStart)
	{
	TIMER_CH1.milosec_count++;	
	if(TIMER_CH1.milosec_count>6)
	{
	 SetViewStart=0;
	 SetViewFlg=1;
	 TIMER_CH1.milosec_count=0;
	}	
	}
   	
	if( TIMER_CH2.timer_start	)
	{
	TIMER_CH2.timer_count_stat=1;
	 TIMER_CH2.sec_count++;	
	if(TIMER_CH2.timer_count_set==0)
	{
	 
	 TIMER_CH2.sec_count=0;
	}
	else
	{
 
   TIMER_CH2.timer_count_set--;
   if(TIMER_CH2.timer_count_set<0)
	 TIMER_CH2.timer_count_set=0;
	 }
	}
  }
}


/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/  
void EXTI15_10_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
  	//usb_check();
		if(exti_trig==0)
		{
		exti_trig=1;
		ext_counter=0;
		//exit_trig_stat=1;
		}
		//OTP_test();
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
}
void EXTI2_IRQHandler(void)
{

if(EXTI_GetITStatus(EXTI_Line2) != RESET)
{
  usb_check();
  EXTI_ClearITPendingBit(EXTI_Line2);
}

}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
