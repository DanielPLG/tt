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
#include <math.h>
#include <stdio.h>
void func_set_ch3(float value );

unsigned long AD7792_SingleConversion(unsigned char ch);
#define PARA_CURR_SET       0.1

#define PARA_CURR_SET_DOWN       0.01

#define PARA_VOLT_SET_PLUS       2
//#define PARA_CURRENT_SET      0.017035
//#define PARA_CURRENT_SET_OFFSET   0.013

#define PARA_CURRENT_SET    0.02145
//#define PARA_CURRENT_SET      0.0204
#define PARA_CURRENT_SET_OFFSET   0.010247
#define PARA_VOLT_SET       33
unsigned char CH1_adc_flg=0;
unsigned char  para_reflseh=0;
unsigned char CH2_adc_flg=0;
unsigned char zero_down_flg=0;
unsigned char zero_down_flg1=0;
extern u8 key_ext_flg;
unsigned char adc_in_flg=0;
float AD_value_v, AD_value_c;;
float AD_value_volt_ch1[TIMES_AVER];
float AD_value_volt_ch2[TIMES_AVER];
float AD_value_curr_ch1[TIMES_AVER];
float AD_value_curr_ch2[TIMES_AVER];
int AD_volt_index1,AD_volt_index2,AD_curr_index1,AD_curr_index2;
int  index_all;
extern u8 adc_get_flg,CH2CV_mode_enable;
  
//void floattostr(char*buf,float tmp);
//#define CONFIG_PARA  1
void init_advalue(void)
{
	int i;
	for(i=0;i<TIMES_AVER;i++)
	{
		AD_value_volt_ch1[i]=0;
		AD_value_volt_ch2[i]=0;
		AD_value_curr_ch1[i]=0;
		AD_value_curr_ch2[i]=0;
	}
	index_all=0;
	AD_volt_index1=0;
	AD_volt_index2=0;
	AD_curr_index1=0;
	AD_curr_index2=0;

}
float mean_value(u8 type,int num)
{
	int i;	
	float tmp,value;

			tmp=0;
	if(type==0)
	{
		for(i=0;i<(TIMES_AVER);i++)
		{
			tmp +AD_value_volt_ch1[i];
			//AD_value_volt_ch1[i]=AD_value_volt_ch1[i+1];
		}
		value=tmp/num;	
	}
	else if(type==1)
	{
		for(i=0;i<(TIMES_AVER);i++)
		{
			tmp +AD_value_volt_ch2[i];
			//AD_value_volt_ch1[i]=AD_value_volt_ch1[i+1];
		}
		value=tmp/num;	
	}
	else if(type==2)
	{
		for(i=0;i<(TIMES_AVER);i++)
		{
			tmp +AD_value_curr_ch1[i];
			//AD_value_volt_ch1[i]=AD_value_volt_ch1[i+1];
		}
		value=tmp/num;	
	}
	else if(type==3)
	{
		for(i=0;i<(TIMES_AVER);i++)
		{
			tmp +AD_value_curr_ch2[i];
			//AD_value_volt_ch1[i]=AD_value_volt_ch1[i+1];
		}
		value=tmp/num;	
	}
	return value;
	
}
void advalue_in(float val,u8 type)
{
	int i;	
	if(type==0)
	{
		AD_volt_index1++;
		if(AD_volt_index1>TIMES_AVER)
			AD_volt_index1=TIMES_AVER;
		for(i=0;i<(TIMES_AVER-1);i++)
		{
			AD_value_volt_ch1[i]=AD_value_volt_ch1[i+1];
		}
			AD_value_volt_ch1[TIMES_AVER-1]=val;
	}
	else if(type==1)
	{
		AD_volt_index2++;
		if(AD_volt_index2>TIMES_AVER)
			AD_volt_index2=TIMES_AVER;
		for(i=0;i<(TIMES_AVER-1);i++)
		{
			AD_value_volt_ch2[i]=AD_value_volt_ch2[i+1];
		}
			AD_value_volt_ch2[TIMES_AVER-1]=val;
	}
	else if(type==2)
	{
		AD_curr_index1++;
		if(AD_curr_index1>TIMES_AVER)
			AD_curr_index1=TIMES_AVER;
		for(i=0;i<(TIMES_AVER-1);i++)
		{
			AD_value_curr_ch1[i]=AD_value_curr_ch1[i+1];
		}
			AD_value_curr_ch1[TIMES_AVER-1]=val;
	}
	else if(type==3)
	{
		AD_curr_index2++;
		if(AD_curr_index2>TIMES_AVER)
			AD_curr_index2=TIMES_AVER;
		for(i=0;i<(TIMES_AVER-1);i++)
		{
			AD_value_curr_ch2[i]=AD_value_curr_ch2[i+1];
		}
			AD_value_curr_ch2[TIMES_AVER-1]=val;
	}
}
#if (OS_VIEW_MODULE == DEF_ENABLED)


/* Private variables ---------------------------------------------------------*/
__align(8) static  OS_STK  OSMeasureTaskStk[APP_TASK_Measure_STK_SIZE];
static  OS_EVENT   *eventAppOSViewTerminalRxMbox;
float AD_value;
float ad_test;
/* Private function prototypes -----------------------------------------------*/
static  void    uctsk_Measure(void *pdata);
static  void    AppOSViewTerminalRx(CPU_INT08U   rx_data);
extern  u8 disp_reflesh;
u8 Adc_get_clear;
u8 LoadFlg=1;
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
	//AD7792_RST();
	//OS_ENTER_CRITICAL();
zero_down_flg=0;
#ifdef	AD7792
	CH1_adc_flg=0;
	para_reflseh=0;
    AD_value=ADS1118_Get_Voltage(0) ;
	//if(key_ext_flg)
	//	return;
	ad_test=AD_value;
    if(AD_value>MAX_OF_ADC)
		AD_value=0;
   // DELAY_MS(1); 
    if(CH1_adc_flg==1)
    	{
    		para_reflseh=1;
		return;
    	}
#else
    AD_value=ADC_Get(0);
	advalue_in(AD_value,0);
	AD_value=mean_value(0,AD_volt_index1);
#endif
	
	//OS_EXIT_CRITICAL();
	if (AD_value<0)
	    return;
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*33;
	ch1_volt_disp=AD_Temp*PARA_K[CH1_VOLT_DISP_CAL].a+PARA_K[CH1_VOLT_DISP_CAL].b;
	ch1_volt_disp_cal=AD_Temp;
	if(ch1_volt_disp<0.0005)
		ch1_volt_disp=0;
	OS_EXIT_CRITICAL();
#ifdef	AD7792
   CH1_adc_flg=0;
  para_reflseh=0;
   AD_value=ADS1118_Get_Voltage(1) ;
   	//if(key_ext_flg)
		//return;
    if(AD_value>MAX_OF_ADC)
		AD_value=0;
 //DELAY_MS(1); 
    if(CH1_adc_flg==1)
    	{
    		para_reflseh=1;
		return;
    	}
#else
    AD_value=ADC_Get(1);
	advalue_in(AD_value,2);
	 AD_value=mean_value(2,AD_curr_index1);
#endif
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*(6.693);
	ch1_curr_disp= (AD_Temp*PARA_K[CH1_CURR_DISP_CAL].a+PARA_K[CH1_CURR_DISP_CAL].b);
	ch1_curr_disp_cal=AD_Temp;
	if(ch1_curr_disp<0.0005)
		{
		//ch1_curr_disp=0;
		zero_down_flg=1;
		}
	if(g_stat.PARA_FLG==0)
	{
		if(ch1_curr_disp<0)
			ch1_curr_disp=0;
	}

	floattostr(Result,ch1_volt_disp);
	ch1_volt_disp=atof(Result);
	floattostr(Result,ch1_curr_disp);
	ch1_curr_disp=atof(Result);
	if(CH1_STAT&&CH2_STAT)
	{
	}
	//else
		//disp_reflesh=1;
	OS_EXIT_CRITICAL();	
}

static void CH2_Measure(void)
{
	OS_CPU_SR  cpu_sr ;
	float tmp;
	char Result[10]={0};
   //  disp_reflesh=1;
	// return;
	CH1_adc_flg=0;
	zero_down_flg1=0;

    AD_value_v=ADS1118_Get_Voltage_CH2(0);
		//if(key_ext_flg)
		//return;
         if(AD_value_v>MAX_OF_ADC)
		AD_value_v=0;
	AD_value=AD_value_v;
    if(CH1_adc_flg==1)
		return;
	if (AD_value<0)
	    return;
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*33;
	ch2_volt_disp_cal=AD_Temp;
	ch2_volt_disp= (AD_Temp*PARA_K[CH2_VOLT_DISP_CAL ].a+PARA_K[CH2_VOLT_DISP_CAL ].b);

	if(ch2_volt_disp<0.0005)
		ch2_volt_disp=0;
	OS_EXIT_CRITICAL();

    AD_value_c=ADS1118_Get_Voltage_CH2(1);
	//	if(key_ext_flg)
	//	return;
         if(AD_value_c>MAX_OF_ADC)
		AD_value_c=0;

AD_value=AD_value_c;

    if(CH1_adc_flg==1)
		return;

	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_ADC)*(6.693);
	ch2_curr_disp_cal=AD_Temp;
	ch2_curr_disp= (AD_Temp*PARA_K[CH2_CURR_DISP_CAL].a+PARA_K[CH2_CURR_DISP_CAL].b);
	if(ch2_curr_disp<0.0005)
		{
		//ch2_curr_disp=0;
		zero_down_flg1=1;
		}
	if(g_stat.PARA_FLG==0)
	{
		if(ch2_curr_disp<0)
			ch2_curr_disp=0;
	}
	floattostr(Result,ch2_volt_disp);
	ch2_volt_disp=atof(Result);
	floattostr(Result,ch2_curr_disp);
	ch2_curr_disp=atof(Result);
	OS_EXIT_CRITICAL();	



	if(g_stat.PARA_FLG)
	{
	  if(para_reflseh)
	  	return;
		if(zero_down_flg1||zero_down_flg)
		{
			if(zero_down_flg1)
				{
				tmp=(ch1_curr_disp+ch2_curr_disp)/2;

				}
			else
				{
				tmp=(ch1_curr_disp+ch2_curr_disp)/2;

				}

			
		}
		else
		{
			tmp=(ch1_curr_disp+ch2_curr_disp)/2;
		}

		if(LoadFlg==0)
		{
			if(tmp>=PARA_CURR_SET )
			{
				LoadFlg=1;
				func_set((CHAN2.volt_set+PARA_VOLT_SET_PLUS),1,CH2_SEL);

				DELAY_MS2(1); 
			}
		}
		else if(LoadFlg==1)
		{
			if(tmp<PARA_CURR_SET_DOWN&& CHAN1.volt_set<PARA_VOLT_SET )
			{
				LoadFlg=0;
				func_set((CHAN2.volt_set),1,CH2_SEL);

				DELAY_MS2(1); 
			}
		}
			if(CH2Mode_flg==1&&para_ch1_on_flg==0)
			{

			}
			if(CH2Mode_flg==0&&para_ch1_on_flg==1)
			{

			}

		{
		ch2_volt_disp=ch1_volt_disp;
		ch1_curr_disp=(ch1_curr_disp+ch2_curr_disp)/2;
		//if(ch1_curr_disp<0)
			ch1_curr_disp=fabs(ch1_curr_disp);
		ch2_curr_disp=ch1_curr_disp;
		para_curr_disp=ch2_curr_disp;
		}

	}
	
	 if(g_stat.SER_FLG)
	{
		//if(para_reflseh)
	  	//return;
		//ch1_curr_disp=ch1_curr_disp;
		ch2_curr_disp=ch1_curr_disp;		
	}	


	if(adc_get_flg==0)
		disp_reflesh=1;
	
}
 void CH3_Measure(void)
{
	OS_CPU_SR  cpu_sr ;
	char Result[10]={0};


	CH1_adc_flg=0;
	para_reflseh=0;
    AD_value=ch3_ad_get_mean(); 
   // AD_value=ADS7040_Read(0xffff) ;
  // DELAY_MS2(20);
  // AD_value=ADS7040_Read(0xffff) ;
  // DELAY_MS2(20);
	ad_test=AD_value;
    if(AD_value>MAX_OF_DA_AD_CH3)
		AD_value=0;


	if (AD_value<0)
	    return;
	OS_ENTER_CRITICAL();
	AD_Temp=(AD_value/MAX_OF_DA_AD_CH3)*3.3*3.13;
	ch3_volt_disp_cal=AD_Temp;
	ch3_volt_disp=AD_Temp*PARA_K[CH3_VOLT_DISP_CAL].a+PARA_K[CH3_VOLT_DISP_CAL].b;

	OS_EXIT_CRITICAL();

}

extern u8  recall_flg_reflsh;
u8   recall_test=0;

static void  uctsk_Measure(void *pdata)
{
	int i;
	INT8U      err;
	OS_CPU_SR  cpu_sr ;
	while (1)
	{
		if(CH3_SET_FLG)
		{
			CH3_SET_FLG=0;
			
			//OSTimeDlyHMSM(0, 0, 0, 100);
			// DELAY_MS2(20);
			if(recall_flg_reflsh==0)
				{
			key_vale=KEY_NONE;
			key_stat=1;
			//recall_test=1;
				}
			else
			{
             recall_flg_reflsh=0;
			}
			func_set_ch3(CHAN3.volt_set );
			func_set_ch3(CHAN3.volt_set );
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}

			//OSTimeDlyHMSM(0, 0, 0, 100);
			DELAY_MS2(20);
			
		}
		if(CH1_SET_FLG)
		{
			CH1_SET_FLG=0;

			#ifdef MASTER_RIGHT	
			if(g_stat.PARA_FLG)
			{
				if (CHAN1.volt_set>=PARA_VOLT_SET )
				{
					func_set((CHAN1.volt_set+PARA_VOLT_SET_PLUS),1,CH1_SEL);
				}
				else
				{
					//LoadFlg=1;
					if (LoadFlg)
						func_set((CHAN1.volt_set+PARA_VOLT_SET_PLUS),1,CH1_SEL);
					else
						func_set(CHAN1.volt_set,1,CH1_SEL);			
				}
			}
			else
			{
				func_set(CHAN1.volt_set,1,CH1_SEL);				
			}
			#else 
			func_set(CHAN1.volt_set,1,CH1_SEL);
			#endif
			if(g_stat.PARA_FLG)
			func_set(CHAN1.curr_set*PARA_K[CH1_CURR_PARA_SET_CAL].a+PARA_K[CH1_CURR_PARA_SET_CAL].b,0,CH1_SEL); 
			else
				{
				if(g_stat.SER_FLG)
				{
					func_set(CHAN1.curr_set,0,CH1_SEL); 
				}
				else
				{
					func_set(CHAN1.curr_set,0,CH1_SEL); 
				}
			
				}
			
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}
		if(CH2_SET_FLG)
		{
			CH2_SET_FLG=0;

			#ifdef MASTER_RIGHT	
			func_set((CHAN2.volt_set),1,CH2_SEL);
			#else
			if(g_stat.PARA_FLG)
			{
				if (CHAN2.volt_set>=PARA_VOLT_SET )
				{
					func_set((CHAN2.volt_set+PARA_VOLT_SET_PLUS),1,CH2_SEL);
					//CH1POWER_ON;
				}
				else
				{
					//LoadFlg=1;
					if (LoadFlg)
						{
							func_set((CHAN2.volt_set+PARA_VOLT_SET_PLUS),1,CH2_SEL);
							//CH1POWER_ON;
						}
					else
						{
							func_set(CHAN2.volt_set,1,CH2_SEL);
							//CH1POWER_OFF;
						}
				}
			}
			else
			{
				func_set(CHAN2.volt_set,1,CH2_SEL);				
			}			
			#endif			
			if(g_stat.PARA_FLG)
			func_set(CHAN2.curr_set,0,CH2_SEL); 
			else
			{
				if(g_stat.SER_FLG)
				{
					func_set(CHAN2.curr_set,0,CH2_SEL);
				}
				else
				{
					func_set(CHAN2.curr_set,0,CH2_SEL);
				}
			
				}
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
		}	
		if(VOLT_REFLESH)
		{
			VOLT_REFLESH=0; 
			if(CHAN_STAT==CH2_SEL)
				func_set(pt_PARA->volt_set,1,CHAN_STAT);
			else
				func_set(pt_PARA->volt_set,1,CHAN_STAT);

			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				if(g_stat.PARA_FLG)
				{
					#ifdef MASTER_RIGHT	
					if (CHAN1.volt_set>=PARA_VOLT_SET )
					{
						func_set((CHAN1.volt_set+PARA_VOLT_SET_PLUS),1,CH1_SEL);
					}
					else
					{
						//LoadFlg=1;
						if (LoadFlg)
							func_set((CHAN1.volt_set+PARA_VOLT_SET_PLUS),1,CH1_SEL);
						else
							func_set(CHAN1.volt_set,1,CH1_SEL);			
					}					
					#else
					if (CHAN2.volt_set>=PARA_VOLT_SET )
					{
						func_set((CHAN2.volt_set+PARA_VOLT_SET_PLUS),1,CH2_SEL);
					}
					else
					{
						//LoadFlg=1;
						if (LoadFlg)
							func_set((CHAN2.volt_set+PARA_VOLT_SET_PLUS),1,CH2_SEL);
						else
							func_set(CHAN2.volt_set,1,CH2_SEL);			
					}					
					#endif
				}
				else
				{
					#ifdef MASTER_RIGHT	
					func_set(CHAN1.volt_set,1,CH1_SEL);
					#else
					func_set(CHAN2.volt_set,1,CH2_SEL);
					#endif
				}
			}
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}
		if(CURR_REFLESH)
		{
			CURR_REFLESH=0;
			func_set(pt_PARA->curr_set,0,CHAN_STAT); 
			if(g_stat.PARA_FLG)
			{
				func_set(pt_PARA->curr_set+CHAN1.curr_set*PARA_CURRENT_SET,0,CHAN_STAT); 
				//CHAN1.curr_set =CHAN1.curr_set+CHAN1.curr_set*0.018125;
			}
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				#ifdef MASTER_RIGHT	
				func_set(CHAN1.curr_set,0,CH1_SEL); 
				#else
				if(g_stat.PARA_FLG)
				{
					func_set(CHAN2.curr_set+CHAN2.curr_set*PARA_CURRENT_SET,0,CH2_SEL); 
				}
				else
				func_set(CHAN2.curr_set,0,CH2_SEL); 
				#endif
			}
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			}
		}	
		if(CH2CV_mode_enable&&g_stat.PARA_FLG)
		{
			CH2CV_mode_enable=0;
			//func_set(CHAN2.volt_set,1,CH2_SEL);	
		}
		if(recall_test)
		{
			//recall_test=0;
			//key_vale=KEY_NONE;
			//key_stat=1;

		}
		if(Adc_get_clear)
			Adc_get_clear=0;
		if(CH1_STAT)
		{
			if(adc_get_flg==1)
				{
					zero_down_flg=0;
					
					CH1_Measure();
					CH2_Measure();
					CH3_Measure();
				}
					adc_get_flg=0;
					zero_down_flg=0;
					
					CH1_Measure();
					CH2_Measure();
					CH3_Measure();
			//DA7562_CLK(0);
			if(key_ext_flg)
				key_ext_flg=0;
		}   
		if(CH2_STAT)
		{

			
			//ch2DA7562_CLK(0);
		}	 
		//OSTimeDlyHMSM(0, 0, 0, 20);	 /* 50 MS  */
	
	}
}


#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
