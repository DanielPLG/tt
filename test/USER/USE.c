#include "Main.h"
#include "common.h"
#include "GUI.H" 
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
//#include "systick.h"
#include "stm32f10x_flash.h"
#define USART_REC_LEN  			200 

typedef  void (*pFuntion)(void);
pFuntion Jump_To_Application;
uint32_t JumpAddress;
uint32_t RamSource;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
u16  USART_COUNTER=0;
u8   USART_RX_IN_FLG=0;  

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configuration the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  	 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
} */

u16 PARA_Saved(uint32_t Address,uint8_t *buf1,u16 size)
{
	u16 i; 
	uint32_t FlashDestination = Address; 
	FLASHStatus = FLASH_ErasePage(FlashDestination);
	RamSource = (uint32_t)buf1;
	for(i=0;i<size;i+= 4)
	{
		FLASH_ProgramWord(FlashDestination, *(uint32_t*)RamSource);
		if (*(uint32_t*)FlashDestination != *(uint32_t*)RamSource)
		{
			return 0;
		}
		FlashDestination +=4;
		RamSource +=4;
	}	  
	return 1;
	//return w25x_write(STATE_ADDRESS, (u8*)&Curstatus, sizeof(Curstatus));
}


void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);


	EXTI_ClearITPendingBit(EXTI_Line11);
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}
void EXTI_Configuration1(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE); 	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);


	EXTI_ClearITPendingBit(EXTI_Line2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}


void PARA_Read(uint32_t Address,uint8_t *buf1,u16 size)
{  
	u16 i;
	for(i=0;i<size;i++)
	{
		*buf1=*(u8*)Address;
		buf1++;
		Address++;
	}
	/*	for(i=0;i<size;i++)
	{
	*buf2=*(u8*)Address;
	buf2++;
	Address++;
	} */
	//return w25x_write(STATE_ADDRESS, (u8*)&Curstatus, sizeof(Curstatus));
}	


void TIME_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 

	TIM_DeInit(TIM2);            //复位定时器

	TIM_TimeBaseStructure.TIM_Period=2000;  //定时器初始值

	TIM_TimeBaseStructure.TIM_Prescaler=(36000-1);         //时钟预分频

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;  // 时钟分割

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);   //初始化定时器的值

	TIM_ClearFlag(TIM2,TIM_FLAG_Update);               //清除定时器中断标志  

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);  //使能中断

	TIM_Cmd(TIM2,ENABLE);            //开启时钟
}


/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
	void USART1_IRQHandler(void)
		{
			u8 Res;
	
			OSIntEnter();	 
	
			if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
			{
					
				Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
				dmm_set_scpi_Interface_type(1);
				USART_RX_IN_FLG=1;
				USART_COUNTER=0;
				if((USART_RX_STA&0x8000)==0)//接收未完成
				{
					//if(USART_RX_STA&0x4000)//接收到了0x0d
					{
						//if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
						//else USART_RX_STA|=0x8000;	//接收完成了 
					}
					
					//else //还没收到0X0D
					{	
						if(Res==0x0a)
							{
							USART_RX_STA|=0x8000;
							USART_RX_IN_FLG=0;
							}
						else
						{
							USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
							USART_RX_STA++;
							if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
						}		 
					}
				}			 
		  } 
			
	 
			OSIntExit();		//退出中断
	
		}


void Scpi_write_data_usart(uint8_t* Buf, uint32_t Len)
{
  uint32_t i;
  
  for (i = 0; i < Len; i++)
  {
		USART_SendData(USART1, *(Buf + i));         //?ò′??ú1·￠?íêy?Y
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//μè′y·￠?í?áê?
  }
}

void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
  NVIC_InitTypeDef        NVIC_InitStructure;	
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO| RCC_APB2Periph_USART1,ENABLE);
  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */	

 GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 		 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;//115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
 // USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_Cmd(USART1, ENABLE);

/* Configure the NVIC Preemption Priority Bits */  
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
/* Enable the USARTy Interrupt */
NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

}


/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Configure the ADC.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	   /* 独立模式 */
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;			   /* 连续多通道模式 */
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	   /* 连续转换 */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  /* 转换不受外界决定 */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		       /* 右对齐 */
	ADC_InitStructure.ADC_NbrOfChannel = 1;					   /* 扫描通道数 */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel15 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5);  /* 通道X,采用时间为55.5周期,1代表规则通道第1个 */	
	ADC_Cmd(ADC1, ENABLE);                 /* Enable ADC1 */                      
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);    /* 使能转换开始 */
}


void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;

	//将外设 RCC寄存器重设为缺省值
	RCC_DeInit();
	HSEStartUpStatus=ERROR;
	while(HSEStartUpStatus==ERROR)
	//设置外部高速晶振（HSE）
	{
		RCC_HSEConfig(RCC_HSE_ON);

		//等待 HSE 起振
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
	}
	if(HSEStartUpStatus == SUCCESS)
	{
		//预取指缓存使能
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//设置代码延时值
		//FLASH_Latency_2  2 延时周期
		FLASH_SetLatency(FLASH_Latency_2);

		//设置 AHB 时钟（HCLK）
		//RCC_SYSCLK_Div1  AHB 时钟 =  系统时钟
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//设置高速 AHB 时钟（PCLK2）
		//RCC_HCLK_Div2  APB1 时钟  = HCLK / 2
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//设置低速 AHB 时钟（PCLK1）
		//RCC_HCLK_Div2  APB1 时钟  = HCLK / 2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		/* ADCCLK = PCLK2/4 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div4);

		// PLLCLK = 8MHz * 9 = 72 MHz
		//设置 PLL 时钟源及倍频系数
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		//使能或者失能 PLL
		RCC_PLLCmd(ENABLE);

		//等待指定的 RCC 标志位设置成功 等待PLL初始化成功
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}


		//设置系统时钟（SYSCLK） 设置PLL为系统时钟源
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		//等待PLL成功用作于系统时钟的时钟源
		//  0x00：HSI 作为系统时钟
		//  0x04：HSE作为系统时钟
		//  0x08：PLL作为系统时钟
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

	//使能或者失能 APB2 外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	// Enable USART1 clocks
	//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* TIM4 clock enable */
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 

	/* Enable DMA1 clock */
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOC clock */
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

	/* Enable PWR clock */
	// RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

}


void ADCLK_Inint(void)
{
	ErrorStatus HSEStartUpStatus;

	//将外设 RCC寄存器重设为缺省值
	RCC_DeInit();
	HSEStartUpStatus=ERROR;
	while(HSEStartUpStatus==ERROR)
	//设置外部高速晶振（HSE）
	{
		RCC_HSEConfig(RCC_HSE_ON);

		//等待 HSE 起振
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
	}
	if(HSEStartUpStatus == SUCCESS)
	{
		//预取指缓存使能
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//设置代码延时值
		//FLASH_Latency_2  2 延时周期
		FLASH_SetLatency(FLASH_Latency_2);

		//设置 AHB 时钟（HCLK）
		//RCC_SYSCLK_Div1  AHB 时钟 =  系统时钟
		RCC_HCLKConfig(RCC_SYSCLK_Div16);

		//设置高速 AHB 时钟（PCLK2）
		//RCC_HCLK_Div2  APB1 时钟  = HCLK / 2
		RCC_PCLK2Config(RCC_HCLK_Div16);

		//设置低速 AHB 时钟（PCLK1）
		//RCC_HCLK_Div2  APB1 时钟  = HCLK / 2
		RCC_PCLK1Config(RCC_HCLK_Div16);

		/* ADCCLK = PCLK2/4 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div4);

		// PLLCLK = 8MHz * 9 = 72 MHz
		//设置 PLL 时钟源及倍频系数
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		//使能或者失能 PLL
		RCC_PLLCmd(ENABLE);

		//等待指定的 RCC 标志位设置成功 等待PLL初始化成功
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}


		//设置系统时钟（SYSCLK） 设置PLL为系统时钟源
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		//等待PLL成功用作于系统时钟的时钟源
		//  0x00：HSI 作为系统时钟
		//  0x04：HSE作为系统时钟
		//  0x08：PLL作为系统时钟
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

	//使能或者失能 APB2 外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
}



void goto_RSTAddress(void)
{
	//SerialPutString("  Execute CLS\r\n\n");
	JumpAddress = *(__IO uint32_t*) (OLDAddress + 4);
	Jump_To_Application = (pFunction) JumpAddress;
	// NVIC_SetVectorTable(NVIC_VectTab_FLASH, NOT_IAP_ADDRESS);
	__set_MSP(*(__IO uint32_t*) Jump_To_Application);
	// NVIC_SetVectorTable(NVIC_VectTab_FLASH, IAP_ADDRESS);
	Jump_To_Application();  

}


