/****************************************Copyright (c)**************************************************
**                             MTI214 CJLU
**
**                      http://HI.BAIDU.COM/CIMCENTER
****                      cimcenter@gmail.com
** This program was produced by the
** IAR Embedded Workbench 4.0 Kickstart 442
** Copyright 2008-2009 MTI214 CJLU
** Chip type           : STM32F103VB
** Program type        : Application
** Clock frequency     : 8.000000 MHz
** Memory model        :
** External SRAM size  :
** Data Stack size     :
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: user_systick.c
**创   建   人: YORK
**最后修改日期: 2009年1月5日
**描        述: user_systick 驱动
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


#include "user_systick.h"
#include "stm32f10x_systick.h"

//#define USART_IT_RXNE                        ((u16)0x0525)
//#include "stm32f10x_usart.h"	
//计算代码运行耗费时间的计数器
vu32 ClickCounter;  			 
static vu32 TimingDelay;

/*******************************************************************************
* Function Name  : SysTick_Config
* Description    : Configures SysTick
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//SysTick设置
void SysTick_Config1(void)
{
    /* Disable SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Disable);

    /* Disable the SysTick Interrupt */
    SysTick_ITConfig(DISABLE);

    /* Configure HCLK clock as SysTick clock source */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

    /* SysTick interrupt each 1000 Hz with HCLK equal to 72MHz */
    SysTick_SetReload(9000);

    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	SysTick_CounterCmd(SysTick_Counter_Enable);
    /* Enable the SysTick Interrupt */
    SysTick_ITConfig(ENABLE);

}


/*******************************************************************************
* Function Name  : Delay_Ms
* Description    : Inserts a delay time.
* Input          : nTime: specifies the delay time length, in milliseconds.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_Ms(u32 nTime)
{							 
    /* Enable the SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Enable);

    TimingDelay = nTime;

    while(TimingDelay != 0);

    /* Disable SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* Clear SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Clear);
}


/*******************************************************************************
* Function Name  : TimingDelayMs_Decrement
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void TimingDelay_Decrement(void)
{
  // this code is for delay ms /////////////////////
    if (TimingDelay != 0x00)
    {
      TimingDelay--;
    }
  /////////////////////////////////////////////////////

    // this code is for count /////////////////////////
    ClickCounter++ ;
}

void Start_Count_Click(void)
{
    /* Enable the SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Enable);

    // init the count register
    ClickCounter = 0;
}

void Stop_Count_Click(void)
{
    /* Disable SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* Clear SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Clear);

}



