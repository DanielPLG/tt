/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_OSView.c
** Descriptions:            The uctsk_OSView application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
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
#include <includes.h>  			   
#include "Main.h"
#include "GUI.H"
//void floattostr(char*buf,float tmp);
//#define CONFIG_PARA  1
#if (OS_VIEW_MODULE == DEF_ENABLED)


/* Private variables ---------------------------------------------------------*/
static  OS_STK  OSMeasureTaskStk[APP_TASK_Measure_STK_SIZE];
static  OS_EVENT   *eventAppOSViewTerminalRxMbox;
float AD_value;
float AD_value_v, AD_value_c;;
/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_Measure(void *pdata);
static  void    AppOSViewTerminalRx(CPU_INT08U   rx_data);
extern  u8 disp_reflesh;
u8 Adc_get_clear;

void  App_OSViewTaskCreate (void)
{
	CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */   	
    os_err = OSTaskCreateExt((void (*)(void *)) uctsk_Measure,
                             (void          * ) 0,
                             (OS_STK        * )&OSMeasureTaskStk[APP_TASK_Measure_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_Measure_PRIO,
                             (INT16U          ) APP_TASK_Measure_PRIO,
                             (OS_STK        * )&OSMeasureTaskStk[0],
                             (INT32U          ) APP_TASK_Measure_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_Measure_PRIO, "Task Measure", &os_err);
	#endif

}



/*
*********************************************************************************************************
*                                          AppTerminalRx()
*
* Description : Callback function for uC/OS-View
*
* Argument(s) : rx_data     The received data.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static void  AppOSViewTerminalRx (CPU_INT08U rx_data)
{

	OSMboxPost(eventAppOSViewTerminalRxMbox, &rx_data);
}
 float AD_Temp;
 float AD_test[10];
 int adc_test_start,ad_num=0;
static void CH1_Measure(void)
{
	OS_CPU_SR  cpu_sr ;
	char Result[10]={0};
	CH2AD7792_RST();
	AD_value=CH2ADC_Get(0);
	if (AD_value<0)
	    return;
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*40;
	ch1_volt_disp=AD_Temp*PARA_K[5].a+PARA_K[5].b;
	if(ch1_volt_disp<0.0005)
		ch1_volt_disp=0;
	OS_EXIT_CRITICAL();
	AD_value=CH2ADC_Get(1);
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*(8.1705);
	ch1_curr_disp= (AD_Temp*PARA_K[7].a+PARA_K[7].b);
	if(ch1_curr_disp<0.0005)
		ch1_curr_disp=0;

	floattostr(Result,ch1_volt_disp);
	ch1_volt_disp=atof(Result);
	floattostr(Result,ch1_curr_disp);
	ch1_curr_disp=atof(Result);
	if(CH1_STAT&&CH2_STAT)
	{
	}
	else
		disp_reflesh=1;
	OS_EXIT_CRITICAL();	
}

static void CH2_Measure(void)
{
	OS_CPU_SR  cpu_sr ;
	char Result[10]={0};
	AD7792_RST();
	AD_value=ADC_Get(0);
	if (AD_value<0)
	    return;
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*40;
	ch2_volt_disp= (AD_Temp*PARA_K[1].a+PARA_K[1].b);
	AD_test[ad_num]=ch2_volt_disp;
/*	ad_num++;
	if (ad_num==5)
		{
			ad_num=0;
			adc_test_start=1;
		}
	if(adc_test_start==1)
		{
			if(	ch2_volt_disp <0.6*CHAN2.volt_set)
				{
					CH3POWER_ON;
					DelayUS(20);
					CH3POWER_OFF;
				}
		} */
	if(ch2_volt_disp<0.0005)
		ch2_volt_disp=0;
	OS_EXIT_CRITICAL();
	AD_value=ADC_Get(1);
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*(8.1705);
	ch2_curr_disp= (AD_Temp*PARA_K[3].a+PARA_K[3].b);
	if(ch2_curr_disp<0.0005)
		ch2_curr_disp=0;

	floattostr(Result,ch2_volt_disp);
	ch2_volt_disp=atof(Result);
	floattostr(Result,ch2_curr_disp);
	ch2_curr_disp=atof(Result);
#ifdef MASTER_RIGHT
	if(g_stat.PARA_FLG)
	{
		ch1_volt_disp=ch2_volt_disp;
		ch1_curr_disp=ch2_curr_disp;
	}
#else
	if(g_stat.PARA_FLG)
	{
		ch2_volt_disp=ch1_volt_disp;
		ch2_curr_disp=ch1_curr_disp;
	}
#endif 
	disp_reflesh=1;
	OS_EXIT_CRITICAL();	
}


static void  uctsk_Measure(void *pdata)
{
	int i;
	INT8U      err;
	OS_CPU_SR  cpu_sr ;
	while (1)
	{
		if(CH1_SET_FLG)
		{
			CH1_SET_FLG=0;
			if(g_stat.PARA_FLG)
				func_set((CHAN1.volt_set+1)*PARA_K[4].a+PARA_K[4].b,1,CH1_SEL);
			else
				func_set(CHAN1.volt_set*PARA_K[4].a+PARA_K[4].b,1,CH1_SEL);
			
			func_set(CHAN1.curr_set,0,CH1_SEL); 
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}
		if(CH2_SET_FLG)
		{
			CH2_SET_FLG=0;
			func_set((CHAN2.volt_set*PARA_K[0].a+PARA_K[0].b),1,CH2_SEL);
			func_set(CHAN2.curr_set,0,CH2_SEL);
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
		}	 
		if(VOLT_REFLESH)
		{
			VOLT_REFLESH=0; 
			if(CHAN_STAT==CH2_SEL)
				func_set(pt_PARA->volt_set*PARA_K[0].a+PARA_K[0].b,1,CHAN_STAT);
			else
				func_set(pt_PARA->volt_set*PARA_K[4].a+PARA_K[4].b,1,CHAN_STAT);
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				if(g_stat.PARA_FLG)
					func_set((CHAN1.volt_set+1)*PARA_K[4].a+PARA_K[4].b,1,CH1_SEL);
				else
					func_set(CHAN1.volt_set*PARA_K[4].a+PARA_K[4].b,1,CH1_SEL);
			}
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}
		if(CURR_REFLESH)
		{
			CURR_REFLESH=0;
			func_set(pt_PARA->curr_set,0,CHAN_STAT); 
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				func_set(CHAN1.curr_set,0,CH1_SEL); 
			}
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}	
		if(Adc_get_clear)
			Adc_get_clear=0;
		if(CH1_STAT)
		{
			CH1_Measure();
		}   
		if(CH2_STAT)
		{
			CH2_Measure();
		}	 
		OSTimeDlyHMSM(0, 0, 0, 10);	 /* 50 MS  */
	}
}


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
