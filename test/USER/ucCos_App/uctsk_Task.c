/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_Task.c
** Descriptions:            The uctsk_Task application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-24
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
#include "timer.h"
#include "common.h"
#include <uCOS-II\Source\ucos_ii.h>
#if ( OS_VIEW_MODULE == DEF_ENABLED )
extern	void OSView_RxTxISRHandler(void);
#endif
u8 led_clear_flg,led_disp_flg,led_reflesh,led_disp_flg1;
u8 disp_reflesh=0;
u8 set_reflesh;
unsigned char restart_enable=0;
u8 main_cur_location;
u8 ch1_cur_location;
u8 cal_cur_location;
u8 ch2_cur_location;
u8 ch3_ChannelSel=0;
u8 restore_disp_flg=0;
u8 fine_set_flg=0;
u8 fine_clear_flg=0;
u8 cal_set_flg=0;
u8 cal_set_reflesh_flg=0;
u8 OTP_beeper_flg=0;
u8 OTP_restore_flg=0;
int OTP_count=0;
u8 SetViewStart=0;
u8 SetViewFlg=0;
u8 ch1_volt_fine_index;
u8 ch2_volt_fine_index;
u8 ch3_volt_fine_index=0;

u8 ch1_curr_fine_index;
u8 ch2_curr_fine_index;
extern u8 CH2Mode_flg,CH2Mode_reflesh,CH3_SET_FLG;
extern u8 SCPI_CH1_SET,SCPI_CH2_SET,SCPI_CH3_SET,SCPI_OVP,SCPI_OCP;

u8  recall_flg_reflsh=0;

u8 cal_volt_fine_index=0;
u8 cal_curr_fine_index=0;
//u8 key=0;
u8 key_stat=0;
u8 key_ext_flg=0;
u8 exit_trig_stat=0;
u8 key_vale=0;
u8 lock_stat=0;

u8 beep_on;
u8 BeeperFlg=0;
u8 lock_key_stat;
extern int ovp_count;
extern u8  ovp_flg;
extern u8  ovp_flash_flg;
extern u8 SCPI_CH1_SET,SCPI_CH2_SET,SCPI_CH3_SET;
extern u8 CH3Mode_flg,CH3Mode_reflesh;

extern unsigned char adc_in_flg;
u8 adc_get_flg=0;
static u8 KeyRelease_flg=0;
int LcdBackCount;
u8 LcdBackFlg;
extern u8  ht1621_flg;
extern u8 Ch1VoltLedBuf[4];
extern u8 Ch3VoltLedBuf[2];

extern u8 Ch2VoltLedBuf[4];
extern u8 Ch1CurrLedBuf[4];
extern u8 Ch2CurrLedBuf[4];
extern u8 Ch1PowerLedBuf[4];
extern u8 Ch2PowerLedBuf[4];
static  OS_EVENT  *KeyRelease;
extern u8 ch1_cur_location,ch2_cur_location,OTPMode_reflesh,OTPMode_flg;
extern u8 file_num,recall_flg;
u8 usb_reflesh,usb_flg;
void init_cur_flg(void);


/* Private variables ---------------------------------------------------------*/
__align(8) static  OS_STK         App_Task_UCGUI_Stk        [APP_TASK_UCGUI_STK_SIZE];
__align(8) static  OS_STK         App_Task_TouchPanel_Stk   [APP_TASK_TouchPanel_STK_SIZE];
__align(8) static  OS_STK         App_TaskBlinkStk          [APP_TASK_BLINK_STK_SIZE];
__align(8) static  OS_STK         App_TaskMeasure          [APP_TASK_MEASURE_STK_SIZE ];
__align(8) static  OS_STK         App_Taskencode_key          [APP_TASK_MEASURE_STK_SIZE ];
__align(8) static  OS_STK         App_Taskencode_key1          [APP_TASK_MEASURE_STK_SIZE ];

__align(8) static  OS_STK         App_Task_Scip_Stk   [APP_TASK_Scpi_STK_SIZE];


/* Private function prototypes -----------------------------------------------*/
       void MainTask         (void);
static void uctsk_Main      (void);
static void uctsk_Led (void);
static void uctsk_Disp      (void);
static  void  uctsk_Measure(void );
void LedDispSelect1(void);
static  void uctsk_Encode_key(void );
static  void uctsk_Encode_key1(void );

void Select_VoltFine(uint8_t channel,uint8_t index);
void Select_CurrFine(uint8_t channel,uint8_t index);
extern void _ExecCalibration (void);
void draw_usb(void);
void clear_usb(void);
static  void  AppTaskKbd (void ); 
void DelBackLcd(void);
void LedDispSelect(void);
void enter_main_page(void)
{
	if(CHAN_SEL==CH1_SEL)
	{
		LED_SER_CH1(1);
		LED_PARA_CH2(0);
		LED_CH3(0);
	}
	else if(CHAN_SEL==CH2_SEL)
	{
		LED_SER_CH1(0);
		LED_PARA_CH2(1);
		LED_CH3(0);

	}
	else
	{
		LED_SER_CH1(0);
		LED_PARA_CH2(0);
		LED_CH3(1);

	}

if( g_stat.OVP_SET_FLG)
{
	 DRAW_OVP_OCP_FLG(0);
	 DRAW_SET_FLG(0);
}
 g_stat.CAL_FLG=0;
 g_stat.OVP_SET_FLG=0;
 cur_page=page_main;
  init_cur_flg();
 if(CHAN_STAT==CH1_SEL)
 	{
 Select_VoltFine(0,ch1_volt_fine_index);
 Select_CurrFine(0,ch1_curr_fine_index);
 	}
 else
 {
 Select_VoltFine(1,ch2_volt_fine_index);
 Select_CurrFine(1,ch2_curr_fine_index);
 }
  ch1_cur_location=main_location_volt;
ch2_cur_location=main_location_volt;
}
void enter_cal_page(void)
{
if( g_stat.OVP_SET_FLG)
{
	 DRAW_OVP_OCP_FLG(0);
	 DRAW_SET_FLG(0);
}
g_stat.OVP_SET_FLG=0;
cur_page=page_calibration;	
 ch1_cur_location=main_location_volt;
ch2_cur_location=main_location_volt;
init_cur_flg();
Select_VoltFine(CHAN_STAT,cal_volt_fine_index);
Select_CurrFine(CHAN_STAT,cal_curr_fine_index);
  BEEP_RING1( 10);
}
extern int ovp_count;
extern u8  ovp_flg;
extern u8  ovp_flash_flg;
void usb_check(void)
{
 	if (!USB_Check)
	{
		usb_flg=1;
		usb_reflesh=1;
		//up_deal();
	}
	else
	{
		usb_flg=0;
		usb_reflesh=1;
	}
}

void enter_ovp_page(void)
{
g_stat.CAL_FLG=0;
cur_page=page_ovp_ocp;
DRAW_SET_FLG(1);
	ovp_flash_flg=0;
	ovp_flg=0;
	ovp_count=0;
 ch1_cur_location=main_location_volt;
ch2_cur_location=main_location_volt;
 if(CHAN_STAT==CH1_SEL)
 	{
 Select_VoltFine(0,ch1_volt_fine_index);
 Select_CurrFine(0,ch1_curr_fine_index);
 	}
 else
 {
 Select_VoltFine(1,ch2_volt_fine_index);
 Select_CurrFine(1,ch2_curr_fine_index);
 }
}
void DealOTPMode(void)
{
		if(OTPMode_flg)
		{
			OTPMode_flg=0;
			OTPMode_reflesh=0;
			DRAW_CLEAR_OTP(0);
	       }
}
static void uctsk_Scpi (void) 
{     
   	for(;;)
   	{  
	   usbtmc_scheduler();
	   deal_scpi_usart_main();
	   OSTimeDlyHMSM(0, 0, 0, 10);	 /* 20 MS  */
    }
}

void  App_UCGUI_TaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; 

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Main,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_UCGUI_Stk[APP_TASK_UCGUI_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_UCGUI_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_UCGUI_PRIO, "Task Main", &os_err);
	#endif 
   
    os_err = OSTaskCreate((void (*)(void *)) uctsk_Led,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_TouchPanel_Stk[APP_TASK_TouchPanel_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_TouchPanel_PRIO  );

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_TouchPanel_PRIO, "TASK  Messages", &os_err);
	#endif


	os_err = OSTaskCreate((void (*)(void *)) uctsk_Disp,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task Disp", &os_err);
	#endif

	/*	os_err = OSTaskCreate((void (*)(void *)) uctsk_Measure,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskMeasure[APP_TASK_MEASURE_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_MEASURE_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_MEASURE_PRIO, "Task MEASURE", &os_err);
	#endif 	*/
    os_err = OSTaskCreate((void (*)(void *)) uctsk_Scpi,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Task_Scip_Stk[APP_TASK_Scpi_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_Scpi_PRIO  );

#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_TouchPanel_PRIO, "TASK Scpi Messages", &os_err);
#endif

			os_err = OSTaskCreate((void (*)(void *)) uctsk_Encode_key,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Taskencode_key [APP_TASK_encodekey_STK_SIZE  - 1],		
                          (INT8U           ) APP_TASK_encodekey_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_encodekey_PRIO , "Task SCAN", &os_err);
	#endif 	
			os_err = OSTaskCreate((void (*)(void *)) uctsk_Encode_key1,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_Taskencode_key1 [APP_TASK_encodekey_STK_SIZE  - 1],		
                          (INT8U           ) APP_TASK_encodekey1_PRIO  );							

#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_encodekey1_PRIO , "Task Encode", &os_err);
#endif	

}							 

u8 key_flg=0;
u8 key_flg1=0;
u8 key_flg2=0;
u8 key_flg3=0;
u8 key_time_flg=0;

static  void uctsk_Encode_key(void )
{
	u8 i;
	INT8U  err;
	u32 cnt;
	u32 key_times;
//	KeyRelease= OSSemCreate(0);
	for(;;)
	{
		key_flg=0;
		key_flg1=0;
		if(!ReadKA)
		{
		      if(!ReadKB)
		      	{
				key_flg=1;
			  }
			  else if(ReadKB)
			  {
				key_flg1=1;
			  }
			  	
			OSTimeDly(5);
			for(i=3; i>0; i--)
			{
				if(!ReadKA)
				{}
				else
				{break;}
			}
			if(i==0)
			{
				cnt=0;
				key_times=0;
				key_time_flg=0;
				while(!ReadKA)
				{
					key_times++;
					
					if(key_times>10000)
					{
						key_time_flg=1;
						OSTimeDly(3);
						//KeyRelease_flg=1;
						//OSSemPend(KeyRelease,0,&err);
					}	
					if(key_time_flg)
					{
						//OSTimeDly(3);
					}
				}
				//OSTimeDly(3);
				if(ReadKB)
				{
					if(key_flg==1&&beep_on==0)
					{
						key_vale=DOWN;
				    	key_stat=1;
						//up_deal();	     
					} 
				}
				else
				{
					if(key_flg1==1&&beep_on==0)
					{
						key_vale= UP;
				    	key_stat=1;
						//down_deal();
					}
				}

				
			}
		}


				
		OSTimeDly(4);
	}
}
static  void uctsk_Encode_key1(void )
{
	u8 i;
	INT8U  err;
	u32 cnt;
	u32 key_times;
//	KeyRelease= OSSemCreate(0);
	for(;;)
	{

		key_flg2=0;
		key_flg3=0;
		 if(!ReadKC)
		{
		      if(!ReadKD)
		      	{
				key_flg2=1;
			  }
			  else if(ReadKD)
			  {
				key_flg3=1;
			  }
			OSTimeDly(5);
			for(i=3; i>0; i--)
			{
				if(!ReadKC)
				{}
				else
				{break;}
			}
			if(i==0)
			{
				cnt=0;
				key_times=0;
				while(!ReadKC)
				{
					key_times++;
					//OSTimeDly(3);
					if(key_times>10000)
					{
						OSTimeDly(3);
						//KeyRelease_flg=1;
						//OSSemPend(KeyRelease,0,&err);
					}	
				}	
				//OSTimeDly(3);
				if(ReadKD)
				{
					if(key_flg2==1&&beep_on==0)
					{
						key_vale=KEY_DOWN_CURRENT;
				    	key_stat=1;
						//up_deal();	     
					} 
				}
				else
				{
					if(key_flg3==1&&beep_on==0)
					{
						key_vale= KEY_UP_CURRENT;
				    	key_stat=1;
						//down_deal();
					}
				}
	
				
			}
		}		
		OSTimeDly(4);
	}
}


void para_start_set(void)
{
	u8 i;
	BeeperFlg=1;
	main_cur_location=main_location_volt;
	ch1_cur_location=main_location_volt;
	ch2_cur_location=main_location_volt;
	ch1_volt_fine_index=1;
	ch2_volt_fine_index=1;
	ch1_curr_fine_index=1;
	ch2_curr_fine_index=1;
	cur_page=page_main;

	pt_PARA=&CHAN1;
	pt_DISP=&DISP_CH1;

	CHAN_STAT=CH1_SEL;




	g_stat.step_flg=0xff;
	g_stat.step_current_flg=0xff;



Select_VoltFine(0,ch1_volt_fine_index);
Select_VoltFine(1,ch2_volt_fine_index);

Select_CurrFine(0,ch1_curr_fine_index);
Select_CurrFine(1,ch2_curr_fine_index);
CHAN1.volt_set=15;
CHAN1.curr_set=3;
CHAN1.Pow_set=CHAN1.volt_set*CHAN1.curr_set;
CHAN1.volt_ovp_set=31;
CHAN1.curr_ocp_set=3.1;
CHAN2.volt_ovp_set=31;
CHAN2.curr_ocp_set=3.1;

CHAN1.power_ocp_set=CHAN1.volt_ovp_set*CHAN1.curr_ocp_set;
CHAN2.power_ocp_set=CHAN2.volt_ovp_set*CHAN2.curr_ocp_set;
CHAN2.volt_set=10;
CHAN2.curr_set=3;
CHAN2.Pow_set=CHAN2.volt_set*CHAN2.curr_set;


CHAN1.cal_volt_set=2;
CHAN1.cal_curr_set=0;
CHAN1.cal_volt_disp=0;
CHAN1.cal_curr_disp=0;

CHAN2.cal_volt_set=0;
CHAN2.cal_curr_set=0;
CHAN2.cal_volt_disp=0;
CHAN2.cal_curr_disp=0;
CHAN3.cal_volt_set=3.3;

g_stat.OVP_SET_FLG=0;
g_stat.OCP_SET_FLG=0;
g_stat.OVP_ON_FLG=0;
g_stat.OCP_ON_FLG=0;
g_stat.set_reflesh=0;
g_stat.ovp_set_reflesh=0;
g_stat.cal_set_reflesh=0;
g_stat.CAL_FLG=0;
g_stat.SetView_FLG=0;
SCPI_OVP=0;
SCPI_OCP=0;


}


static  void  uctsk_Measure(void )
{
	u8 i,j;
	OS_CPU_SR  cpu_sr ;
	u16 offset;
	for(;;)
	{
		BEEP_ON;
		OSTimeDlyHMSM(0, 0, 0, 50);	 
		BEEP_OFF;
		OSTimeDlyHMSM(0, 0, 0, 50);	 	
		//OSTimeDlyHMSM(0, 0, 0, 150);	 
	}
}

 void set_key_ALL_ON(void)
 {
	key_vale=KEY_SCPI;
	key_stat=1;
 }
 extern u8  ext_trig_flg;
 void deal_real_disp(void)
 {
 	//if(OTP_restore_flg==0)
 		{
		if(OTPMode_flg==0)
		{
			if(ReadCC_OTP)
			{
				OSTimeDly(1);
				if(ReadCC_OTP)
				{
					OTPMode_flg=1;
					OTPMode_reflesh=1;
				}
			}
		} 
		/*else if(OTPMode_flg==1)
		{
			if(!ReadCC_OTP)
			{
				OSTimeDly(1);
				if(!ReadCC_OTP)
				{
					OTPMode_flg=0;
					OTPMode_reflesh=1;
				}
			}
		}	*/
 		}
	if(ext_trig_flg==1)
	{
			if(!ReadKey_trig)
			{
				OSTimeDlyHMSM(0, 0, 0, 20);
				if(!ReadKey_trig)
				{
					ext_trig_flg=0;
					exit_trig_stat=1;
				}
			}
	}
	else if(ext_trig_flg==0)
	{
			if(ReadKey_trig)
			{
				OSTimeDlyHMSM(0, 0, 0, 20);
				if(ReadKey_trig)
				{
					ext_trig_flg=1;
					exit_trig_stat=1;
					//CH1Mode_reflesh=1;
				}
			}
	}
	if(usb_flg==0)
	{
		if(USB_Check)
		{
			OSTimeDly(1);
			if(USB_Check)
			{
				usb_flg=1;
				usb_reflesh=1;
			}
		}
	} 
	else if(usb_flg==1)
	{
		if(!USB_Check)
		{
			OSTimeDly(1);
			if(!USB_Check)
			{
				usb_flg=0;
				usb_reflesh=1;
			}
		}
	}

		if(CH1Mode_flg==0)
		{
			if(ReadCC_CH1)
			{
				OSTimeDly(1);
				if(ReadCC_CH1)
				{
					CH1Mode_flg=1;
					CH1Mode_reflesh=1;
				}
			}
		} 
		else if(CH1Mode_flg==1)
		{
			if(!ReadCC_CH1)
			{
				OSTimeDly(1);
				if(!ReadCC_CH1)
				{
					CH1Mode_flg=0;
					CH1Mode_reflesh=1;
				}
			}
		}
		if(CH2Mode_flg==0)
		{
			if(ReadCC_CH2)
			{
				OSTimeDly(1);
				if(ReadCC_CH2)
				{
					CH2Mode_flg=1;
					CH2Mode_reflesh=1;
				}
			}
		} 
		else if(CH2Mode_flg==1)
		{
			if(!ReadCC_CH2)
			{
				OSTimeDly(1);
				if(!ReadCC_CH2)
				{
					CH2Mode_flg=0;
					CH2Mode_reflesh=1;
				}
			}
		}

 if(CH3Mode_flg==0)
 {
	 if(ReadCC_CH3)
	 {
		 OSTimeDly(1);
		 if(ReadCC_CH3)
		 {
			 CH3Mode_flg=1;
			 CH3Mode_reflesh=1;
		 }
	 }
 } 
 else if(CH3Mode_flg==1)
 {
	 if(!ReadCC_CH3)
	 {
		 OSTimeDly(1);
		 if(!ReadCC_CH3)
		 {
			 CH3Mode_flg=0;
			 CH3Mode_reflesh=1;
		 }
	 }
 }

 }
 extern unsigned char cal_mode_enable;
static  void  uctsk_Main(void )
{
	u8 i;
	u8 key=0;
	u32 j,k,l;
	u32 KeyPressTime;
	while(1)
	{
		//OSTimeDly(10);
		if(KeyRelease_flg)
		{
			if(ReadKA)
			{
				KeyRelease_flg=0;
				OSSemPost(KeyRelease); 
			}
		}
		key = key_scan();
		if(key != 0)
		{
			for(l=0;l<0x10000;l++);       //Modify by shawge
			// GUI_StoreKeyMsg(key,1);
			OSTimeDly(20);
			key = key_scan();
			/*for(i=3; i>0; i--)
			{
				if((key = key_scan()) == 0)
					break;
				
				OSTimeDly(10);
			}	 */

			if(key != 0)
			{
				// key_stat=key;
				//GUI_StoreKeyMsg(key,1);
				if(key!=KEY_FINE&&key!=KEY_FINE_CURRENT&&key!=KEY_CH1&&key!=KEY_CH2&&key!=KEY_CH3)
					{
					if(fine_set_flg)
						{
				fine_clear_flg=1;
				OSTimeDly(20);
						}
					}
				//DealOTPMode();
				KeyPressTime=0;
					key_vale=key;
					key_stat=1;
					//key_ext_flg=1;
				while( (key = key_scan()) != 0 )
				{
					//if(lock_key_stat==0)
					{
					if(key_vale==KEY_ALL_ON)
					{
					//KeyPressTime++;
					//if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					//KeyPressTime=KEY_LONG_PRESS_TIME;
					key_vale=KEY_ALL_ON;
					
					OSTimeDly(5);
					deal_real_disp();
					//lock_key_stat=1;
					//break;
					}
					}					
					else if(key_vale==KEY_FINE)
					{
					{
					key_vale=KEY_FINE;
					OSTimeDly(100);

					}
					}
					else if(key_vale==KEY_CH3)
					{
					{
					key_vale=KEY_CH3;
					OSTimeDly(100);

					}
					}
						else if(key_vale==KEY_FINE_CURRENT)
					{
					//KeyPressTime++;
					//if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					//KeyPressTime=KEY_LONG_PRESS_TIME;
					key_vale=KEY_FINE_CURRENT;
					//BEEP_OFF;
					OSTimeDly(100);
					BEEP_OFF;
					//lock_key_stat=1;
					//break;
					}
					}
					else if(key_vale==KEY_CH1)
					{
					KeyPressTime++;
					if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					KeyPressTime=0;
					key_vale=KEY_SER;
					
					OSTimeDly(100);
					//lock_key_stat=1;
					//break;
					}
					}
					else if(key_vale==KEY_SER)
					{
						OSTimeDly(100);
					}
					else if(key_vale==KEY_PARA)
					{
						OSTimeDly(100);
					}
						else if(key_vale==KEY_CAL)
					{
						OSTimeDly(100);
					}
					else if(key_vale==KEY_CH2)
					{
					KeyPressTime++;
					if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					    KeyPressTime=0;
					    key_vale=KEY_PARA;
						OSTimeDly(100);
					   // lock_key_stat=1;
					//break;
					}
					}
					else if(key_vale==KEY_LOCK)
					{
					KeyPressTime++;
					if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					    KeyPressTime=0;
					    key_vale=KEY_UNLOCK;
					   OSTimeDly(100);
						//OSTimeDly(10);
					   // lock_key_stat=1;
					//break;
					}
					}
					else if(key_vale==KEY_SETVIEW)
					{
					KeyPressTime++;
					if(KeyPressTime>KEY_CAL_PRESS_TIME)
					{
					    KeyPressTime=0;
					   if(cal_mode_enable)
					    	key_vale=KEY_CAL;
					   else
					   	{
					   	key_vale=KEY_NONE;
					   	restart_enable=1;
					   	}
					    OSTimeDly(100);
						//OSTimeDly(10);
					   // lock_key_stat=1;
					//break;
					}
					}	
					else if(key_vale==KEY_BEEPER)
					{
					KeyPressTime++;
					if(KeyPressTime>KEY_LONG_PRESS_TIME)
					{
					    KeyPressTime=0;
					    key_vale=KEY_UNBEEPER;
						OSTimeDly(100);
					   // lock_key_stat=1;
					//break;
					}
					}					

					else if(key_vale==KEY_OVP)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_OVP_SET;
						OSTimeDly(100);
    					    //lock_key_stat=1;
    					//break;
    					}
    				}
					else if(key_vale==KEY_OCP)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_OCP_SET;
						OSTimeDly(100);
    					    //lock_key_stat=1;
    					//break;
    					}
    				}	
					else if(key_vale==KEY_M1)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_M_CALL1;
							OSTimeDly(100);
    					}
    				}
					else if(key_vale==KEY_M2)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_M_CALL2;
							OSTimeDly(100);
    					}
    				}
					else if(key_vale==KEY_M3)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_M_CALL3;
							OSTimeDly(100);
    					}
    				}
					else if(key_vale==KEY_M4)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_M_CALL4;
							OSTimeDly(100);
    					}
    				}
					else if(key_vale==KEY_M5)
    				{
    					KeyPressTime++;
    					if(KeyPressTime>KEY_LONG_PRESS_TIME)
    					{
    					    KeyPressTime=0;
    					    key_vale=KEY_M_CALL5;
							OSTimeDly(100);
    					}
    				}




					else if(key_vale==KEY_OVP_SET)
					{
						OSTimeDly(100);
					}
						else if(key_vale==KEY_OCP_SET)
					{
						OSTimeDly(100);
					}
						else if(key_vale==KEY_UNBEEPER)
					{
						OSTimeDly(100);
					}
						else if(key_vale==KEY_UNLOCK)
					{
						OSTimeDly(100);
					}
					}
					// GUI_StoreKeyMsg(key,1);
					//  OSTimeDly(100);
				}

				 
			}
		} 

		deal_real_disp();

		OSTimeDly(10);
	}
}
void Select_VoltFine(uint8_t channel,uint8_t index)
{
	unsigned  char ledreg;
    switch (index)
    {

		case 0:
			//volt_step=1.00;
			if(channel==0)
			{
			    CHAN1.volt_set_step=10.00; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=10.00;
			}
			else
			{
				CHAN3.volt_set_step=10.00;
			}

			break;
		case 1:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.00; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.00;
			}
			else
			{
				CHAN3.volt_set_step=1.00;
			}

			break;
		case 2:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-1; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-1; 
			}
			else
			{
				CHAN3.volt_set_step=1.000000e-1;
			}

			break;
		case 3:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-2; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-2; 
			}	
			else
			{
				CHAN3.volt_set_step=1.000000e-2;
			}

			break;

		case 4:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-3; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-3; 
			}
			else
			{
				CHAN3.volt_set_step=1.000000e-3;
			}

			break;
		case 5:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-4; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-4; 
			}
			else
			{
				CHAN3.volt_set_step=1.000000e-4;
			}

			break;
		case 6:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-5;  
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-5; 
			}
			else
			{
				CHAN3.volt_set_step=1.000000e-5;
			}

		case 7:
			if(channel==0)
			{
			    CHAN1.volt_set_step=1.000000e-6; 
			}
			else if(channel==1)
			{
			    CHAN2.volt_set_step=1.000000e-6; 
			}
			else
			{
				CHAN3.volt_set_step=1.000000e-7;
			}			
		default:

			break;
	}
}
void Select_CurrFine(uint8_t channel,uint8_t index)
{
	unsigned  char ledreg;
    switch (index)
    {

		case 0:
			//volt_step=1.00;
			if(channel==0)
			{
			    CHAN1.curr_set_step=1.00; 
			}
			else
			{
			    CHAN2.curr_set_step=1.00;
			}

			break;
		case 1:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.1; 
			}
			else
			{
			    CHAN2.curr_set_step=0.1;
			}			
			break;
		case 2:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.01; 
			}
			else
			{
			    CHAN2.curr_set_step=0.01;
			}	
			break;
		case 3:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.001; 
			}
			else
			{
			    CHAN2.curr_set_step=0.001;
			}				
			break;

		case 4:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.0001; 
			}
			else
			{
			    CHAN2.curr_set_step=0.0001;
			}
			break;
		case 5:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.00001; 
			}
			else
			{
			    CHAN2.curr_set_step=0.00001;
			}	
			break;
		case 6:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.000001; 
			}
			else
			{
			    CHAN2.curr_set_step=0.000001;
			}
		case 7:
			if(channel==0)
			{
			    CHAN1.curr_set_step=0.0000001; 
			}
			else
			{
			    CHAN2.curr_set_step=0.0000001;
			}	
			break;
		default:

			break;
	}
}
void clear_disp_shan_flg(void)
{
	led_disp_flg=0;
	led_clear_flg=0;
}
u8 reg;
void init_cur_flg(void)
{
	ch1_volt_fine_index=0;
	ch2_volt_fine_index=0;
	ch3_volt_fine_index=0;
	ch1_curr_fine_index=0;
	ch2_curr_fine_index=0;
	cal_volt_fine_index=0;
	cal_curr_fine_index=0;
	Select_VoltFine(0,cal_volt_fine_index);
	Select_VoltFine(1,cal_volt_fine_index);
	Select_VoltFine(2,cal_volt_fine_index);
	Select_CurrFine(0,cal_curr_fine_index);
	Select_CurrFine(1,cal_curr_fine_index);
}
static void uctsk_Disp (void) 
{                 
	CPU_SR         cpu_sr; 
	u8 i,regt;
	float ad_value;
	for(;;)
	{  
		if(key_stat)
		{
            //BEEP_RING(50);
			key_stat=0;
			DealOTPMode();
			//key_ext_flg=0;
			if(ext_trig_flg==0)
			{
				goto goto_cur_page;
			}
			if(OTPMode_flg)
			{	     
				goto goto_cur_page;
			}
			if(main_info_flg)
			{	     
				goto goto_cur_page;
			}
			if(key_vale== KEY_LOCK)
			{
				key_ext_flg=1;
				 if(cur_page==page_calibration)
					{
					BEEP_RING1( 10);
					 //if(cur_page==page_ovp_ocp||cur_page==page_calibration)
					 if(CHAN_STAT==CH1_SEL)
					{
					ch1_volt_fine_index=0;
					ch2_volt_fine_index=0;
					ch1_curr_fine_index=0;
					ch2_curr_fine_index=0;
					PARA_K[CH1_CURR_SET_CAL].a=1;
					PARA_K[CH1_CURR_SET_CAL].b=0;
					PARA_K[CH1_CURR_DISP_CAL].a=1;
					PARA_K[CH1_CURR_DISP_CAL].b=0;
					PARA_K[CH1_CURR_PARA_SET_CAL].a=1;
					PARA_K[CH1_CURR_PARA_SET_CAL].b=0;
					}
					 
					 else
					 {
					ch1_volt_fine_index=0;
					ch2_volt_fine_index=0;
					ch1_curr_fine_index=0;
					ch2_curr_fine_index=0;
					PARA_K[CH2_CURR_SET_CAL].a=1;
					PARA_K[CH2_CURR_SET_CAL].b=0;
					PARA_K[CH2_CURR_DISP_CAL].a=1;
					PARA_K[CH2_CURR_DISP_CAL].b=0;
					 }
                        //break;
					}
				 else
				 	{
				 BEEP_RING(50);
				key_vale=0;
				lock_stat=1;
				//lock_stat=(~lock_stat);
				if(lock_stat)
				{

					//LED1_LOCK(1);
					DRAW_LOCK_FLG(1);
					LED_LOCK(1);
					// goto goto_cur_page;
				}
				else
				{


					//LED1_LOCK(0);
				}
				lock_key_stat=0;
				 	}
			}
			else if(key_vale== KEY_UNLOCK)
			{
				key_ext_flg=1;
				BEEP_RING(50);
				key_vale=0;
               		 lock_stat=0;
				LED_LOCK(0);
				DRAW_LOCK_FLG(0);
				//LED1_LOCK(0);
			}
			if(lock_stat)
			{	     
				goto goto_cur_page;
			}
			if(main_info_flg)
			{	     
				goto goto_cur_page;
			}
			/*====LED-ON=======*/
			switch (key_vale)
			{
				case KEY_CAL:
					key_ext_flg=1;
					g_stat.CAL_FLG=(~g_stat.CAL_FLG);
					if(g_stat.CAL_FLG)
					{
					    if(CHAN_SEL==CH3_SEL)
					    {
 							LED_SER_CH1(0);
							LED_PARA_CH2(0);
						}
						
						enter_cal_page();
					    g_stat.cal_set_reflesh=1;
						cal_cur_location=0;
					}
					else
					{
					   // cur_page=page_main;
					  // init_cur_flg();
						enter_main_page();
					     BEEP_RING1( 10);
					    g_stat.set_reflesh=1;
					}
					//left_deal();
					//read_ht1621_all();
					break;
				case RIGHT:
					//right_deal(); 
					break;
				case KEY_UP_CURRENT:
						if(main_cur_location==main_location_volt)
						break;
					/*if(cur_page==page_calibration)
					{
						if(main_cur_location==main_location_volt)
						break;
					}
					else
					{
					    main_cur_location=main_location_curr;
					}*/
					//main_cur_location=main_location_curr;
					key_ext_flg=1;
					if(fine_set_flg&&CHAN_SEL==CH3_SEL)
					{
						break;
					}

					up_deal();
					//read_ht1621_all();
					break;
				case KEY_DOWN_CURRENT:
					
						{
					if(main_cur_location==main_location_volt)
						break;
						}
					/*if(cur_page==page_calibration)
					{
						if(main_cur_location==main_location_volt)
						break;
					}
					else
					{
					    main_cur_location=main_location_curr;
					}*/
					key_ext_flg=1;
					if(fine_set_flg&&CHAN_SEL==CH3_SEL)
					{
						break;
					}

					down_deal();
					//read_ht1621_all();
					break;
				case UP:
					if(CHAN_SEL!=CH3_SEL)
						{
					if(main_cur_location==main_location_curr||main_cur_location==main_location_cal)
						break;
						}

					key_ext_flg=1;
					if(fine_set_flg&&CHAN_SEL==CH3_SEL&&cur_page!=page_calibration)
					{
						TIMER_CH1.sec_count=0;
						fine_set_flg=1;

						if(ch3_volt_fine_index==0)
						{
							CHAN3.volt_set=CHAN3.volt_set+0.1;
						}
						else if(ch3_volt_fine_index==1)
							{
							CHAN3.volt_set=CHAN3.volt_set+1;
						}
						if(CHAN3.volt_set>Volt_max_ch3)
							{
						  CHAN3.volt_set=Volt_max_ch3;
						  BEEP_RING(50);
							}
						CH3_value_set(CHAN3.volt_set);
						CH3_SET_FLG=1;
						HT1621_Disp_CH3_volt(CHAN3.volt_set);
						//func_set_ch3(CHAN3.volt_set );
						break;
					}

					up_deal();
					//read_ht1621_all();
					break;
				case DOWN:
					    if(CHAN_SEL!=CH3_SEL)
					    {
							if(main_cur_location==main_location_curr||main_cur_location==main_location_cal)
							break;
					    }

					key_ext_flg=1;
					if(fine_set_flg&&CHAN_SEL==CH3_SEL&&cur_page!=page_calibration)
					{
					           
            			 TIMER_CH1.sec_count=0;
						 fine_set_flg=1;
						if(ch3_volt_fine_index==0)
						{
							CHAN3.volt_set=CHAN3.volt_set-0.1;
						}
						else if(ch3_volt_fine_index==1)
							{
							CHAN3.volt_set=CHAN3.volt_set-1;
						}
						if(CHAN3.volt_set<0)
							{
						  CHAN3.volt_set=0;
						   BEEP_RING(50);
							}
						CH3_value_set(CHAN3.volt_set);
						//func_set_ch3(CHAN3.volt_set );
						HT1621_Disp_CH3_volt(CHAN3.volt_set);
						CH3_SET_FLG=1;
						break;
					}

					down_deal();
					//read_ht1621_all();
					break;
				case KEY_OK:
					//ok_deal();
					break;
				case KEY_OVP_SET:
					key_ext_flg=1;
					BEEP_RING(50);
					g_stat.OVP_SET_FLG=(~g_stat.OVP_SET_FLG);
					if(g_stat.OVP_SET_FLG)
					{
                        enter_ovp_page();
						g_stat.ovp_set_reflesh=1;
						DRAW_OVP_OCP_FLG(1);
					}
					else
					{
						
                       		  enter_main_page();
					   DRAW_OVP_OCP_FLG(0);
					   DRAW_SET_FLG(0);
					   g_stat.set_reflesh=1;
					}
					//read_ht1621_all();
					break;
				case KEY_OCP_SET:
					key_ext_flg=1;
					BEEP_RING(50);
					g_stat.OVP_SET_FLG=(~g_stat.OVP_SET_FLG);
					if(g_stat.OVP_SET_FLG)
					{
                        enter_ovp_page();
						DRAW_OVP_OCP_FLG(1);
						g_stat.ovp_set_reflesh=1;
					}
					else
					{						
                       			enter_main_page();
					   DRAW_OVP_OCP_FLG(0);
					   DRAW_SET_FLG(0);
					   g_stat.set_reflesh=1;
					}	
					//read_ht1621_all();
					break;					
				case KEY_PARA:
					key_ext_flg=1;
					BEEP_RING(50);
						g_stat.ALL_ON_FLG=0x0ff;
						deal_ALL_ON();
					if(g_stat.SER_FLG )
					{
						CH1POWER_OFF;
						CH2POWER_OFF;

						g_stat.SER_FLG=0;
						DRAW_CLEAR_SER(1);

						DELAY_SER_OFF;
					}
					g_stat.PARA_FLG=(~g_stat.PARA_FLG);
					if(SCPI_PARA_SET)
					{
						SCPI_PARA_SET=0;
						if(SCPI_PARA_FLG)
							g_stat.PARA_FLG=0xff;
						else
							g_stat.PARA_FLG=0;
					}

							if(fine_set_flg==1)
						{
							fine_set_flg=0;
							restore_disp_flg=1;
							clear_disp_shan_flg();
						}			
					enter_main_page();
					 SetReflsehAll();
					deal_PARA();
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
			//read_ht1621_all();
					break;
				case KEY_SER:
					key_ext_flg=1;
					BEEP_RING(50);
						g_stat.ALL_ON_FLG=0x0ff;
						deal_ALL_ON();
					if(g_stat.PARA_FLG )
					{
						CH1POWER_OFF;
						CH2POWER_OFF;
						g_stat.PARA_FLG=0;
						DRAW_CLEAR_PARA(1);


						DELAY_PARA_OFF;
						DELAY_PARA1_OFF;
						DELAY_PARA2_OFF;
					}
					g_stat.SER_FLG=(~g_stat.SER_FLG);
					if(SCPI_SER_SET)
					{
						SCPI_SER_SET=0;
						if(SCPI_SER_FLG)
							g_stat.SER_FLG=0xff;
						else
							g_stat.SER_FLG=0;
					}

						if(fine_set_flg==1)
						{
							clear_disp_shan_flg();
							fine_set_flg=0;
							restore_disp_flg=1;
						}
					enter_main_page();
					 SetReflsehAll();
					deal_SER();
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
			//read_ht1621_all();
					break;
			case KEY_NONE:
			
					if(recall_SET_FLG)
					{
						//recall_SET_FLG=0;
						//break;
					}
				recall_count=0;
				Select_recall(recall_count);
				HT1621_Clear_Save_count();
				break;
			case KEY_M1:
				if(cur_page==page_calibration)
				{
				CalParam_Store();
				BEEP_RING1( 10);
				break;
				}

				PARA_Read(PARA_ADDRESS_(0),&para_store, sizeof(para_store));
				if(para_store.flg==0xaa)
				{
					recall_count=1;
					Select_recall(recall_count);
					HT1621_Disp_Save_count(1);
					_recall();
		
				}
				else
				{
	
				}

				break;
			case KEY_M2:
				if(cur_page==page_calibration)
					break;
				PARA_Read(PARA_ADDRESS_(1),&para_store, sizeof(para_store));
				if(para_store.flg==0xaa)
				{
					recall_count=2;
					Select_recall(recall_count);
					HT1621_Disp_Save_count(2);
					_recall();
		
				}
				else
				{
	
				}

				break;
			case KEY_M3:
				if(cur_page==page_calibration)
					break;

				PARA_Read(PARA_ADDRESS_(2),&para_store, sizeof(para_store));
				if(para_store.flg==0xaa)
				{
					recall_count=3;
					Select_recall(recall_count);
					HT1621_Disp_Save_count(3);
					_recall();
		
				}
				else
				{
	
				}

				break;
			case KEY_M4:
				if(cur_page==page_calibration)
					break;

				PARA_Read(PARA_ADDRESS_(3),&para_store, sizeof(para_store));
				if(para_store.flg==0xaa)
				{
					recall_count=4;
					Select_recall(recall_count);
				    HT1621_Disp_Save_count(4);
					_recall();
		
				}
				else
				{
	
				}

				break;
			case KEY_M5:
				if(cur_page==page_calibration)
					break;

				PARA_Read(PARA_ADDRESS_(4),&para_store, sizeof(para_store));
				if(para_store.flg==0xaa)
				{
					recall_count=5;
					Select_recall(recall_count);
				    HT1621_Disp_Save_count(5);
					_recall();
		
				}
				else
				{
	
				}

				break;
			case KEY_M_CALL1:
				if(cur_page==page_calibration)
					break;

				if(set_store(para_store,PARA_ADDRESS_(0)))
					{
					}
				HT1621_Clear_Save_count();
				Select_recall(0);
				BEEP_RING(50);
				
				break;
			case KEY_M_CALL2:
				if(cur_page==page_calibration)
					break;

				if(set_store(para_store,PARA_ADDRESS_(1)))
					{
					}
					HT1621_Clear_Save_count();
					Select_recall(0);
					BEEP_RING(50);
					break;
			case KEY_M_CALL3:
				if(cur_page==page_calibration)
					break;

				if(set_store(para_store,PARA_ADDRESS_(2)))
					{
					}
				     HT1621_Clear_Save_count();
					 Select_recall(0);
					BEEP_RING(50);
					break;
			case KEY_M_CALL4:
				if(cur_page==page_calibration)
					break;

				if(set_store(para_store,PARA_ADDRESS_(3)))
					{
					}
				    HT1621_Clear_Save_count();
					Select_recall(0);
					BEEP_RING(50);
					break;
			case KEY_M_CALL5:
				if(cur_page==page_calibration)
					break;

				if(set_store(para_store,PARA_ADDRESS_(4)))
					{
					}
					HT1621_Clear_Save_count();
					Select_recall(0);
					BEEP_RING(50);
					break;


			case KEY_LOCK:
					break;
			case KEY_CH1:
					key_ext_flg=1;
					BEEP_RING(50);
					if(fine_set_flg==1&&CHAN_SEL==CH3_SEL)
					{
						fine_set_flg=0;
						CHAN_SEL=CHAN_STAT;
					}

					 if(cur_page==page_calibration)
					 {
					 if(CHAN_SEL==CH1_SEL)
					 {
					 	SET_REFLESH_CAL_PAGE();
                        			cal_cur_location=(cal_cur_location+1)%3;
						main_cur_location=cal_cur_location;
						if(main_cur_location==0)
						{
							cal_cur_location=main_location_volt;
							main_cur_location=main_location_volt;
						}
					  ch1_cur_location=main_cur_location;
					  fine_set_flg=1;
					TIMER_CH1.sec_count=0;
					  
					  Select_VoltFine(0,cal_volt_fine_index);
					  Select_CurrFine(0,cal_curr_fine_index);
					 }
					 else
					 {
					    pt_PARA=&CHAN1;
					    pt_DISP=&DISP_CH1;
		

					    CHAN_STAT=CH1_SEL;
						CHAN_SEL=CH1_SEL;
					    LED_SER_CH1(1);
					    LED_PARA_CH2(0);
						LED_CH3(0);
					  fine_set_flg=0;
					   SetReflsehAll();
					 }
						break;
					 }
					 if(g_stat.ALL_ON_FLG)
					 	{
					 if(g_stat.PARA_FLG)
					 {
						ch2_curr_disp=para_curr_disp;
						ch1_curr_disp=para_curr_disp;
					 }
					 disp_reflesh=1;
					 	}
					 if(CHAN_STAT==CH2_SEL)
					 {
					 	if(fine_set_flg)
					 		{
					 			restore_disp_flg=1;
					 			clear_disp_shan_flg();
					 			fine_set_flg=0;
					 			SET_REFLESH_SET_PAGE(main_cur_location);
					 	
					 		}
					 }
					pt_PARA=&CHAN1;
					pt_DISP=&DISP_CH1;
					CHAN_STAT=CH1_SEL;
					CHAN_SEL=CH1_SEL;
					LED_SER_CH1(1);
					LED_PARA_CH2(0);
					LED_CH3(0);
					
					if(CHAN_STAT==CH1_SEL)
					{
						main_cur_location=ch1_cur_location;
					  if(fine_set_flg)
					  {
                         		if(ch1_cur_location==main_location_volt)
                         		{
                            		ch1_cur_location=main_location_curr;
					}
					  else
					  {
                            		ch1_cur_location=main_location_volt;
					  }						 
					  }
					  else
					  {
                            		ch1_cur_location=main_location_volt;
					  }
                      		if(g_stat.ALL_ON_FLG==0)
                      		{
                        			SET_REFLESH_SET_PAGE(main_cur_location);
					  }
					//clear_disp_shan_flg();
					  fine_set_flg=1;
					TIMER_CH1.sec_count=0;
					  
					 main_cur_location=ch1_cur_location;
					  SET_REFLESH_SET_PAGE(main_cur_location);

					  break;
					}
						
					//SET_REFLESH();
					
	



					//Channel_Change();
					//main_cur_location=ch1_cur_location;

					//SET_REFLESH();
				
					//read_ht1621_all();
					break;
				case KEY_CH2:
					key_ext_flg=1;
					BEEP_RING(50);
					if(fine_set_flg==1&&CHAN_SEL==CH3_SEL)
					{
						fine_set_flg=0;
						CHAN_SEL=CHAN_STAT;
					}

					 if(cur_page==page_calibration)
					 {
					 if(CHAN_SEL==CH2_SEL)
					 {
					 	SET_REFLESH_CAL_PAGE();
                        			cal_cur_location=(cal_cur_location+1)%3;
						main_cur_location=cal_cur_location;
						if(main_cur_location==0)
						{
							cal_cur_location=main_location_volt;
							main_cur_location=main_location_volt;
						}
					  	ch2_cur_location=main_cur_location;
					  	fine_set_flg=1;
						//  fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					  
					 	Select_VoltFine(0,cal_volt_fine_index);
					  	Select_CurrFine(0,cal_curr_fine_index);		
					 	SET_REFLESH_CAL_PAGE();
					 }
					 else
					 {
						pt_PARA=&CHAN2;
						pt_DISP=&DISP_CH2;
	
						CHAN_STAT=CH2_SEL;
						CHAN_SEL=CH2_SEL;
						//Channel_Change();
						LED_SER_CH1(0);
						LED_PARA_CH2(1);
						LED_CH3(0);
					  	fine_set_flg=0;
					   	SetReflsehAll();
					 }
						break;
					 }

					 if(g_stat.ALL_ON_FLG)
					 	{
					 if(g_stat.PARA_FLG)
					 {
						ch2_curr_disp=para_curr_disp;
						ch1_curr_disp=para_curr_disp;
					 }
					 disp_reflesh=1;
					 	}
					 if(CHAN_STAT==CH1_SEL)
					 {
					 		if(fine_set_flg)
					 		{
					 			fine_set_flg=0;
					 			SET_REFLESH_SET_PAGE(main_cur_location);
					 	
					 		}
					 }
					pt_PARA=&CHAN2;
					pt_DISP=&DISP_CH2;

					CHAN_STAT=CH2_SEL; 
					CHAN_SEL=CH2_SEL;
					LED_SER_CH1(0);
					LED_PARA_CH2(1);
					LED_CH3(0);
					main_cur_location=main_location_volt;	
					//fine_set_flg=0;
					if(CHAN_STAT==CH2_SEL)
					{
						main_cur_location=ch2_cur_location;
					  if(fine_set_flg)
					{
					if(ch2_cur_location==main_location_volt)
					{
					ch2_cur_location=main_location_curr;
					}
					else
					{
					ch2_cur_location=main_location_volt;
					}						 
					}
					  else
					  {
                      	ch2_cur_location=main_location_volt;
					  }					  
                      if(g_stat.ALL_ON_FLG==0)
                      {
                      	SET_REFLESH_SET_PAGE(main_cur_location);
					  }
					  fine_set_flg=1;
                      TIMER_CH1.sec_count=0;
					//  clear_disp_shan_flg();
					 main_cur_location=ch2_cur_location;
                     SET_REFLESH_SET_PAGE(main_cur_location);
                    if(cur_page==page_main)
                    {
					// SET_REFLESH();	
                    }
					else if(cur_page==page_ovp_ocp)
					{
						SET_REFLESH_OVP_PAGE();
					}				  
					  break;
					}						
						
			

				
					
			
					//read_ht1621_all();
					break;
				case KEY_CH3:
					
					
					BEEP_RING(50);
					//main_cur_location=main_location_volt;
					if(cur_page==page_calibration)
					{
					  if(CHAN_SEL==CH3_SEL)
					  	{
					  	fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					  	break;
					  	}
						CHAN_SEL=CH3_SEL;
						LED_SER_CH1(0);
						LED_PARA_CH2(0);
						 LED_CH3(1);
						Select_VoltFine(2,cal_volt_fine_index);
						fine_set_flg=0;
						
						SetReflsehAll();


					  break;
					}
                    if(CHAN_SEL==CH1_SEL||CHAN_SEL==CH2_SEL)
                    {
						if(fine_set_flg)
						{
							fine_set_flg=0;
							SET_REFLESH_SET_PAGE(main_cur_location);
						
						}
						LED_SER_CH1(0);
						LED_PARA_CH2(0);
						LED_CH3(1);

						CHAN_SEL=CH3_SEL;
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					else
					{
                      if(fine_set_flg)
                      {
                      	fine_set_flg=0;
  						ch3_ChannelSel=(ch3_ChannelSel+1)%4;
						para_store.ch3_Sel=ch3_ChannelSel;
                    	Select_CH3(ch3_ChannelSel);    
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
						//ch3_volt_fine_index=(ch3_volt_fine_index+1)%2;
					  }
					  else
					  {
						  LED_SER_CH1(0);
						  LED_PARA_CH2(0);
						  LED_CH3(1);

 						CHAN_SEL=CH3_SEL;
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					  }
					}
					//CHAN_SEL=CH3_SEL;
					//fine_set_flg=1;
					//TIMER_CH1.sec_count=0;
					

					

				    //if(g_stat.ALL_ON_FLG==0)
					{
						HT1621_Disp_CH3_volt(CHAN3.volt_set);
					}
			        para_store.ch3_Sel=ch3_ChannelSel;

			        if(set_store(para_store,Delault_Set_ADDRESS))
			        {
			        }	
					//read_ht1621_all();
					break;
				case KEY_CH2_ON:
					key_ext_flg=1;
					BEEP_RING(50);
					CH2_STAT=(~CH2_STAT) ;

					if(g_stat.PARA_FLG||g_stat.SER_FLG)
					{
						CH1_STAT=CH2_STAT;
					}
					if(CH1_STAT==0&&CH2_STAT==0&&g_stat.ch3_stat==0)
					{
						g_stat.ALL_ON_FLG=0xff;
						deal_ALL_ON();
					}
					else  if(CH1_STAT&&CH2_STAT&&g_stat.ch3_stat)
					{
						g_stat.ALL_ON_FLG=0x00;
						deal_ALL_ON();
					}
					else
					{
						deal_CH2(1);
						if(g_stat.PARA_FLG||g_stat.SER_FLG)
						{
							deal_CH1(1);
						}
					}
					//read_ht1621_all();
					break;
				case KEY_CH1_ON:
					key_ext_flg=1;
					BEEP_RING(50);
					CH1_STAT=(~CH1_STAT) ;

					if(g_stat.PARA_FLG||g_stat.SER_FLG)
					{
						CH2_STAT=CH1_STAT;
					}
					if(CH1_STAT==0&&CH2_STAT==0&&g_stat.ch3_stat==0)
					{
						g_stat.ALL_ON_FLG=0xff;
						deal_ALL_ON();
					}
					else  if(CH1_STAT&&CH2_STAT&&g_stat.ch3_stat)
					{
						g_stat.ALL_ON_FLG=0x00;
						deal_ALL_ON();
					}
					else
					{
						deal_CH1(1);
						if(g_stat.PARA_FLG||g_stat.SER_FLG)
						{
							deal_CH2(1);
						}
					}
					//read_ht1621_all();
					break;
				case KEY_ALL_ON :
					key_ext_flg=1;
					BEEP_RING(50);
					if(OTPMode_flg)
						break;
						if(fine_set_flg==1)
						{
							fine_set_flg=0;
							restore_disp_flg=1;
						}
					deal_ALL_ON();

					//read_ht1621_all();
					break;	
				case KEY_FINE:
					key_ext_flg=1;
					BEEP_RING(50);

					 if(cur_page==page_calibration&&CHAN_SEL==CH3_SEL)
					 {
						 SET_REFLESH_SET_PAGE(ch1_cur_location);
						  cal_volt_fine_index=(cal_volt_fine_index+1)%6;
						  Select_VoltFine(2,cal_volt_fine_index);
						  fine_set_flg=1;
						  TIMER_CH1.sec_count=0;

						break;
					 }
					if(fine_set_flg==0&&CHAN_SEL==CH3_SEL)
					{
						//CHAN_SEL=CHAN_STAT;
						TIMER_CH1.sec_count=0;
						HT1621_Disp_CH3_volt(CHAN3.volt_set);
						fine_set_flg=1;
						break;
					}
					if(fine_set_flg&&CHAN_SEL==CH3_SEL)
					{
						TIMER_CH1.sec_count=0;
						HT1621_Disp_CH3_volt(CHAN3.volt_set);
						fine_set_flg=1;
						ch3_volt_fine_index=(ch3_volt_fine_index+1)%2 ;
						break;
					}
					if(fine_set_flg==0)
						{
						if(CHAN_STAT==CH1_SEL)
						{
							ch1_cur_location=main_location_volt;
							 main_cur_location=ch1_cur_location;
						}
						else
						{
							ch2_cur_location=main_location_volt;
							 main_cur_location=ch2_cur_location;
						}
							ch1_cur_location=main_location_volt;
							 main_cur_location=ch1_cur_location;
							 SET_REFLESH_SET_PAGE(main_cur_location);
					 		fine_set_flg=1;
							TIMER_CH1.sec_count=0;
							break;
						}
					if(fine_set_flg==0)
						break;	
		        if(cur_page==page_calibration)
		        {
					if(CHAN_SEL==CH1_SEL)
					{
						SET_REFLESH_SET_PAGE(ch1_cur_location);
						cal_volt_fine_index=(cal_volt_fine_index+1)%6 ;
					    Select_VoltFine(0,cal_volt_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					else if(CHAN_SEL==CH2_SEL)
					{
						SET_REFLESH_SET_PAGE(ch2_cur_location);
						cal_volt_fine_index=(cal_volt_fine_index+1)%6;
					    Select_VoltFine(1,cal_volt_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					else
					{
					   SET_REFLESH_SET_PAGE(ch1_cur_location);
						cal_volt_fine_index=(cal_volt_fine_index+1)%6;
					    Select_VoltFine(2,cal_volt_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;

					}
				}
				else
					{
					if(CHAN_STAT==CH1_SEL)
					{
						SET_REFLESH_SET_PAGE(ch1_cur_location);
						ch1_volt_fine_index=(ch1_volt_fine_index+1)%4;
					    Select_VoltFine(0,ch1_volt_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					else
					{
						SET_REFLESH_SET_PAGE(ch2_cur_location);
						ch2_volt_fine_index=(ch2_volt_fine_index+1)%4;
					    Select_VoltFine(1,ch2_volt_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					}
				//read_ht1621_all();
					break;
				case KEY_FINE_CURRENT:
					key_ext_flg=1;
					BEEP_RING(50);
					if(CHAN_SEL==CH3_SEL)
						break;
					if(fine_set_flg==0&&CHAN_SEL==CH3_SEL)
					{
						CHAN_SEL=CHAN_STAT;
					}
					if(fine_set_flg&&CHAN_SEL==CH3_SEL)
					{
					  break;
					}
					if(fine_set_flg==0)
						{
							if(CHAN_STAT==CH1_SEL)
						{
							ch1_cur_location=main_location_curr;
							 main_cur_location=ch1_cur_location;
						}
						else
						{
							ch2_cur_location=main_location_curr;
							 main_cur_location=ch2_cur_location;
						}
							//ch1_cur_location=main_location_curr;
							// main_cur_location=ch1_cur_location;
							 SET_REFLESH_SET_PAGE(main_cur_location);
					 		fine_set_flg=1;
							TIMER_CH1.sec_count=0;
							break;
						}
					if(fine_set_flg==0)
						break;	
		        if(cur_page==page_calibration)
		        {
		          if(CHAN_SEL==CH3_SEL)
				  	break;
					if(CHAN_STAT==CH1_SEL)
					{
						SET_REFLESH_SET_PAGE(ch1_cur_location);
						cal_curr_fine_index=(cal_curr_fine_index+1)%6 ;
					    Select_CurrFine(0,cal_curr_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
					else
					{
						SET_REFLESH_SET_PAGE(ch2_cur_location);
						cal_curr_fine_index=(cal_curr_fine_index+1)%6;
					    Select_CurrFine(1,cal_curr_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
					}
				}
				else
					{
					if(CHAN_STAT==CH1_SEL)
					{
						
						ch1_curr_fine_index=(ch1_curr_fine_index+1)%4 ;
					    Select_CurrFine(0,ch1_curr_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
						SET_REFLESH_SET_PAGE(ch1_cur_location);
					}
					else
					{
						
						ch2_curr_fine_index=(ch2_curr_fine_index+1)%4 ;
					    	Select_CurrFine(1,ch2_curr_fine_index);
						fine_set_flg=1;
						TIMER_CH1.sec_count=0;
						SET_REFLESH_SET_PAGE(ch2_cur_location);
					}	
					}
					//read_ht1621_all();
					break;
				case KEY_SETVIEW:
					key_ext_flg=1;
					BEEP_RING(50);
					 if(cur_page==page_ovp_ocp)
					   break;
					 if(cur_page==page_calibration)
					 {
						//PARA_K[CH1_CURR_PARA_SET_CAL].a=1;
						//PARA_K[CH1_CURR_PARA_SET_CAL].b=0;
						break;
					 }
					if(g_stat.ALL_ON_FLG==0)
					{
                       		 break;
					}
					g_stat.SetView_FLG=(~g_stat.SetView_FLG);
					if(g_stat.SetView_FLG)
					{
                    			  enter_main_page();
					  g_stat.set_reflesh=1;
					  DRAW_SET_FLG(1);
					  LED_SETVIEW(1);
					  TIMER_CH1.milosec_count=0;
					  SetViewStart=1;
					}
					else
					{
						DRAW_SET_FLG(0);
                        			g_stat.set_reflesh=0;
						LED_SETVIEW(0);
						
					}
					//read_ht1621_all();
					break;
				case KEY_OVP:
					key_ext_flg=1;
					BEEP_RING(50);
					deal_OVP();
					//read_ht1621_all();
					break;
				case KEY_OCP:
					key_ext_flg=1;
					BEEP_RING(50);
					deal_OCP();
					//read_ht1621_all();
					break;
				case KEY_BEEPER:
					key_ext_flg=1;
					BEEP_RING(50);
					 if(cur_page==page_calibration)
    				{
    				BEEP_RING1( 10);
                         if(CHAN_SEL==CH1_SEL)
                         {
  					ch1_volt_fine_index=0;
					ch2_volt_fine_index=0;
					ch1_curr_fine_index=0;
					ch2_curr_fine_index=0;
    					    PARA_K[CH1_VOLT_SET_CAL].a=1;
    					    PARA_K[CH1_VOLT_SET_CAL].b=0;
    					    PARA_K[CH1_VOLT_DISP_CAL].a=1;
    					    PARA_K[CH1_VOLT_DISP_CAL].b=0;

                         }
    					 else if(CHAN_SEL==CH2_SEL)
    					 {
   					ch1_volt_fine_index=0;
					ch2_volt_fine_index=0;
					ch1_curr_fine_index=0;
					ch2_curr_fine_index=0;
    					    PARA_K[CH2_VOLT_SET_CAL].a=1;
    					    PARA_K[CH2_VOLT_SET_CAL].b=0;
	    					PARA_K[CH2_VOLT_DISP_CAL].a=1;
    					    PARA_K[CH2_VOLT_DISP_CAL].b=0;
    					 }
						 else
						 {
							 PARA_K[CH3_VOLT_SET_CAL].a=1;
							 PARA_K[CH3_VOLT_SET_CAL].b=0;
							 PARA_K[CH3_VOLT_DISP_CAL].a=1;
							 PARA_K[CH3_VOLT_DISP_CAL].b=0;


						 }
                            break;
    				}
					LED_BEEP(1);
					BeeperFlg=1;
					DRAW_BEEPER_FLG(BeeperFlg);
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
			//read_ht1621_all();
					break;
				case KEY_UNBEEPER:
					key_ext_flg=1;
					BeeperFlg=0;
					LED_BEEP(0);
					DRAW_BEEPER_FLG(BeeperFlg);
			if(set_store(para_store,Delault_Set_ADDRESS))
			{
			} 
			//read_ht1621_all();
					break;					
				case KEY_SCPI:

					break;							
				default:
					OSTimeDlyHMSM(0, 0, 0, 50);	 /* 50 MS  */
					break;
			}
		}
		goto_cur_page:      
		switch(cur_page)
		{
			case page_main:
				PAGE_MAIN();
				break;
			case page_ovp_ocp:
				PAGE_OVP_MAIN();
				break;
			case page_calibration: 
				PAGE_CAL_MAIN();
				break;
			case page_store: 
				break;
			case page_version:
			// PAGE_VERSION();
				break;
			default:
				break;
		}
		if(SetViewFlg)
		{
			key_ext_flg=1;
			DRAW_SET_FLG(0);
			LED_SETVIEW(0);
            		SetViewFlg=0;
			g_stat.SetView_FLG=0;
		}
        LedDispSelect1();
         if(fine_clear_flg==1)
         {
		fine_clear_flg=0;
		if(fine_set_flg==1&&CHAN_SEL==CH3_SEL)
		{
			clear_disp_shan_flg();
			fine_set_flg=0;
			
			ad_value=CHAN3.volt_set;
			if(CH1_STAT)
			{
				ad_value=ch3_volt_disp;
			}

            HT1621_Disp_CH3_volt(ad_value);
		}
		else if(fine_set_flg)
			{
				clear_disp_shan_flg();
				fine_set_flg=0;
				SET_REFLESH_SET_PAGE(main_cur_location);
					 	
			}
		 }
		if(usb_reflesh)
		{
			usb_reflesh=0;
		if(usb_flg)
		  {
		  	
			clear_usb();
		    }
		 else
		 {
		  	draw_usb();
		 }
		}
		if(SCPI_OVP)
		{
          SCPI_OVP=0;
		  if(g_stat.OVP_ON_FLG)
		  {
			  LED_OVP(1);
		  }
		  else
		  {
			  LED_OVP(0);
		  }

		}
		if(SCPI_OCP)
		{
          SCPI_OCP=0;
		  if(g_stat.OCP_ON_FLG)
		  {
			  LED_OCP(1);
		  }
		  else
		  {
			  LED_OCP(0);
		  }

		}

		if(SCPI_CH2_SET)
		{
			SCPI_CH2_SET=0;
			CH2_SET_FLG=1;
			key_vale=KEY_NONE;
			key_stat=1;

			if(CH1_STAT==0&&cur_page==page_main)
			{
				fine_set_flg=0;
				SetReflsehAll();
			}

		}
		if(SCPI_CH1_SET)
		{
			CH1_SET_FLG=1;
			SCPI_CH1_SET=0;
			key_vale=KEY_NONE;
			key_stat=1;

			if(CH1_STAT==0&&cur_page==page_main)
			{
				fine_set_flg=0;
				SetReflsehAll();
			}
		}
		if(SCPI_CH3_SET)
		{
			CH3_SET_FLG=1;
			SCPI_CH3_SET=0;
			key_vale=KEY_NONE;
			key_stat=1;

			if(CH1_STAT==0)
			{
				fine_set_flg=0;
				//CH3_value_set(CHAN3.volt_set);
				HT1621_Disp_CH3_volt(CHAN3.volt_set);
			}
			CH3_value_set(CHAN3.volt_set);
			CH3_SET_FLG=1;
		}
		if(recall_flg)
		{
			recall_flg=0;
			PARA_Read(PARA_ADDRESS_(file_num),&para_store, sizeof(para_store));
			if(para_store.flg==0xaa)
			{
				Select_recall(file_num+1);
				HT1621_Disp_Save_count(file_num+1);
				_recall();
			}
			else
			{
				ERR_Num=2;
				ERR_Flg=1;
				beep_on=1;
			}
		}


		if(ovp_flg)
		{
			ovp_flg=0;
			ovp_flash_flg=0;
			ovp_count=0;
			 DRAW_OVP_OCP_FLG(0);
		}

		if(exit_trig_stat==1)
		{
			exit_trig_stat=0;
			if(ext_trig_flg==0)
			{
				g_stat.ALL_ON_FLG=0;
			}
			else
			{
				g_stat.ALL_ON_FLG=0xff;
			}
			key_ext_flg=1;
			deal_ALL_ON();
		}

		if(OTPMode_reflesh)
		{
			key_ext_flg=1;
			OTPMode_reflesh=0;
			DRAW_CLEAR_OTP(OTPMode_flg);
		if(OTPMode_flg)
			{
				g_stat.ALL_ON_FLG=0xff;
				//LED_CH1_CV_OFF ;
				//LED_CH2_CV_OFF;
				//CH1_STAT=0;
				//CH2_STAT=0;
				//CH1POWER_OFF; 
				//CH2POWER_OFF; 
				//g_stat.ch3_stat=0;
				//g_stat.set_reflesh=1;
				//LED_ON_OFF(0);
				//HT1621_Disp_ON_FLG(0);
				deal_ALL_ON ();
				BEEP_RING(10);
				TIMER_CH2.sec_count=0;
				OTP_restore_flg=1;
				OTP_count=0;
			}
		else
			{
			//g_stat.ALL_ON_FLG=0;
			//deal_ALL_ON ();
			//TIMER_CH2.sec_count=0;

		}
			
			//Mode_Disp(1);
		}
	if(OTP_restore_flg)
		{
	 if(OTP_count>10)
	 {
		OTP_restore_flg=0;
		OTP_count=0;
	 }
		}
	if(OTPMode_flg)
	{

	  if(led_disp_flg1)
	  	DRAW_CLEAR_OTP(1);
	  else
	  	DRAW_CLEAR_OTP(0);
	}
	//DRAW_CLEAR_OTP(1);
		if(OTP_beeper_flg)
		{
		    OTP_beeper_flg=0;
			BEEP_RING(10);
		}
		if(CH1Mode_reflesh)
		{
			CH1Mode_reflesh=0;
			Mode_Disp(1);
		}
		if(CH2Mode_reflesh)
		{
			CH2Mode_reflesh=0;
			Mode_Disp(2);
		}
		if(CH3Mode_reflesh)
		{
			CH3Mode_reflesh=0;
			if(CH1_STAT)
			{
				if(CH3Mode_flg==0)
				{
					HT1621_set_bit(26,1,0);
					HT1621_clear_bit(26,1,1);
				}
				else
				{
					HT1621_set_bit(26,1,1);
					HT1621_clear_bit(26,1,0);				
				}
			}
		}
		if(beep_on)
		{
			beep_on=0;
			BEEP_RING(50);
		}

	/*	if(recall_flg)
		{
			recall_flg=0;
			PARA_Read(PARA_ADDRESS_(file_num),&para_store, sizeof(para_store));
			if(para_store.flg==0xaa)
			{
				_recall();
			}
			else
			{
				ERR_Num=2;
				ERR_Flg=1;
				beep_on=1;
			}
		}*/
		OSTimeDlyHMSM(0, 0, 0, 10);	 /* 50 MS  */
	}
}
void OTP_test(void)
{
OTPMode_flg=1;
OTPMode_reflesh=1;
}

extern u8 Ch1VoltLedBuf[4];
u8  ch1VoltledshanIn=0;
u8  ch1CurrledshanIn=0;
u8  ch2VoltledshanIn=0;
u8  ch2CurrledshanIn=0;
void LedDispSelect1(void)
{
	u8 test,addr;
	u8 clear=0x08;
	u8 index,cur;
	u8 i;
 //  if(adc_in_flg)
   	//return;
  if(CHAN_SEL==CH3_SEL&&cur_page!=page_calibration)
  {
	  {
		  
			 addr=27+ch3_volt_fine_index*2;
			 test=Ch3VoltLedBuf[ch3_volt_fine_index];
	  
			if(led_clear_flg==1&&led_disp_flg==0&&led_reflesh==1)
			{
			  led_reflesh=0;

			  if(ch3_volt_fine_index==0)
				  clear=0x10;
			  else
				  clear=0x00;
			  key_ext_flg=1;
			  WriteAll_1621Byte_CH2(addr,&clear,1);
			  return;
			}
			if(led_clear_flg==0&&led_disp_flg==1&&led_reflesh==1)
			{
			  led_reflesh=0;
			  key_ext_flg=1;
				  WriteAll_1621Byte_CH2(addr,&test,1);
				  return;
			}
			if(restore_disp_flg==1)
			{
			  restore_disp_flg=0;
			  clear_disp_shan_flg();
			  key_ext_flg=1;
			  for(i=0;i<2;i++)
			  {
				 // test=Ch3VoltLedBuf[i];
				  //WriteAll_1621Byte_CH2(27+i,Ch3VoltLedBuf,1);
			  }
			  WriteAll_1621Byte_CH2(27,Ch3VoltLedBuf,2);

			  restore_disp_flg=0;
			  return;
			}
			  
				  //ch1_cur_location=main_location_curr;
			   }

  }
  if(cur_page==page_calibration)
  {
            if(cal_set_reflesh_flg)
            {
				cal_set_reflesh_flg=0;
				g_stat.cal_set_reflesh=1;
            }			
	if(CHAN_STAT==CH1_SEL)
	{
        cur=ch1_cur_location;
	}
	else
	{
         cur=ch2_cur_location;
	}
    {
		  
     if(cur==main_location_volt)
       {
		if(cal_volt_fine_index==0||cal_volt_fine_index==4||cal_volt_fine_index==5)
			clear=0x00;
		else
			clear=0x08;
   	   if(cal_volt_fine_index<4)
   	   {
        addr=cal_volt_fine_index*2;
        test=Ch1VoltLedBuf[cal_volt_fine_index];

	    if(led_clear_flg==1&&led_disp_flg==0)
	     {
	  	    ch1VoltledshanIn=1;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH1(addr,&clear,1);
	     }
	     if(led_clear_flg==0&&led_disp_flg==1)
	    {
	  	    ch1VoltledshanIn=0;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH1(addr,&test,1);
	         }
	        if(ch1VoltledshanIn==1&&led_disp_flg==0&&led_clear_flg==0)
	        {
            ch1VoltledshanIn=0;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH1(addr,&test,1);
	        }		
   	   }
	   else
	   {
        addr=(cal_volt_fine_index-4)*2;
        test=Ch2VoltLedBuf[cal_volt_fine_index-4];

	    if(led_clear_flg==1&&led_disp_flg==0)
	     {
	  	    ch1VoltledshanIn=1;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH2(addr,&clear,1);
	     }
	     if(led_clear_flg==0&&led_disp_flg==1)
	    {
	  	    ch1VoltledshanIn=0;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH2(addr,&test,1);
	         }
	        if(ch1VoltledshanIn==1&&led_disp_flg==0&&led_clear_flg==0)
	        {
            ch1VoltledshanIn=0;
			//key_ext_flg=1;
	        WriteAll_1621Byte_CH2(addr,&test,1);
	        }	
	   }
            //ch1_cur_location=main_location_curr;
         }
      
      else if(cur==main_location_curr)
      {
		if( cal_curr_fine_index==5||cal_curr_fine_index==4)
			clear=0x00;
		else
			clear=0x08;	  	
           //ch1_cur_location=main_location_volt;
       if(cal_curr_fine_index<4)
       {
       addr=cal_curr_fine_index*2+9;
       test=Ch1CurrLedBuf[cal_curr_fine_index];

	  if(led_clear_flg==1&&led_disp_flg==0)
	  {
	  	ch1CurrledshanIn=1;
		//key_ext_flg=1;
	    WriteAll_1621Byte_CH1(addr,&clear,1);
	  }
	  if(led_clear_flg==0&&led_disp_flg==1)
	  {
	  	ch1CurrledshanIn=0;
		//key_ext_flg=1;
	    WriteAll_1621Byte_CH1(addr,&test,1);
	  }
	  if(ch1CurrledshanIn==1&&led_disp_flg==0&&led_clear_flg==0)
	  {
       ch1CurrledshanIn=0;
	  // key_ext_flg=1;
	   WriteAll_1621Byte_CH1(addr,&test,1);
	  }	
       }
	   else
	   {
       addr=(cal_curr_fine_index-4)*2+8;
       test=Ch2CurrLedBuf[cal_curr_fine_index-4];
		if(cal_curr_fine_index==6)
		{
	        if(g_stat.ALL_ON_FLG)
	         {
	            SET_BIT(test, LED_BIT3); 
		       // CLEAR_BIT(test, LED_BIT3); 
	            SET_BIT(clear, LED_BIT3); 
		       // CLEAR_BIT(clear, LED_BIT3); 
				
	         }
	        else
	        {
	           // SET_BIT(test, LED_BIT3); 
		        CLEAR_BIT(test, LED_BIT3); 
	           // SET_BIT(clear, LED_BIT3); 
		        CLEAR_BIT(clear, LED_BIT3); 
	        }
		}
		if(cal_curr_fine_index==7)
		{
	        if(g_stat.ALL_ON_FLG)
	         {
	           // SET_BIT(test, LED_BIT3); 
		        CLEAR_BIT(test, LED_BIT3); 
	           // SET_BIT(clear, LED_BIT3); 
		        CLEAR_BIT(clear, LED_BIT3); 
				
	         }
	        else
	        {
	            SET_BIT(test, LED_BIT3); 
		       // CLEAR_BIT(test, LED_BIT3); 
	            SET_BIT(clear, LED_BIT3); 
		       // CLEAR_BIT(clear, LED_BIT3); 
	        }
		}
	  if(led_clear_flg==1&&led_disp_flg==0)
	  {
	  	ch1CurrledshanIn=1;
		//key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&clear,1);
	  }
	  if(led_clear_flg==0&&led_disp_flg==1)
	  {
	  	ch1CurrledshanIn=0;
		//key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&test,1);
	  }
	  if(ch1CurrledshanIn==1&&led_disp_flg==0&&led_clear_flg==0)
	  {
       ch1CurrledshanIn=0;
	  // key_ext_flg=1;
	   WriteAll_1621Byte_CH2(addr,&test,1);
	  }	
       }
      }
   
      
    }
	  }
  else
  {
	if(CHAN_STAT==CH1_SEL)
    {
	  
     if(ch1_cur_location==main_location_volt)
       {
   	
       addr=ch1_volt_fine_index*2;
       test=Ch1VoltLedBuf[ch1_volt_fine_index];

	  if(led_clear_flg==1&&led_disp_flg==0&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch1VoltledshanIn=1;
		if(ch1_volt_fine_index==0)
			clear=0x00;
		else
			clear=0x08;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH1(addr,&clear,1);
	  }
	  if(led_clear_flg==0&&led_disp_flg==1&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch1VoltledshanIn=0;
		key_ext_flg=1;
	    	WriteAll_1621Byte_CH1(addr,&test,1);
	  }
	  if(restore_disp_flg==1)
	  {
		restore_disp_flg=0;
		clear_disp_shan_flg();
		key_ext_flg=1;
		for(i=0;i<4;i++)
		{
			test=Ch1VoltLedBuf[i];
			WriteAll_1621Byte_CH1(i*2,&test,1);
		}
		restore_disp_flg=0;
	  }
		
            //ch1_cur_location=main_location_curr;
         }
      
      else if(ch1_cur_location==main_location_curr)
      {
	  	
           //ch1_cur_location=main_location_volt;
           addr=ch1_curr_fine_index*2+9;
           test=Ch1CurrLedBuf[ch1_curr_fine_index];

	  if(led_clear_flg==1&&led_disp_flg==0&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch1CurrledshanIn=1;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH1(addr,&clear,1);
	  }
	  if(led_clear_flg==0&&led_disp_flg==1&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch1CurrledshanIn=0;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH1(addr,&test,1);
	  }
	  if(restore_disp_flg==1)
	  {
	  	clear_disp_shan_flg();
		restore_disp_flg=0;
		key_ext_flg=1;
		for(i=0;i<4;i++)
		{
			test=Ch1CurrLedBuf[i];
			WriteAll_1621Byte_CH1(i*2+9,&test,1);
		}
	  }	   
      }
    
      
    }
	else
	{

		  
             if(ch2_cur_location==main_location_volt)
           {
    	   	    addr=ch2_volt_fine_index*2;
    	   	    test=Ch2VoltLedBuf[ch2_volt_fine_index];

	  if(led_clear_flg==1&&led_disp_flg==0&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	if(ch2_volt_fine_index==1)
			clear=0x08;
		else
			clear=0x00;
	  	ch2VoltledshanIn=1;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&clear,1);
	  }
	  	  if(led_clear_flg==0&&led_disp_flg==1&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch2VoltledshanIn=0;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&test,1);
	  }
	  if(restore_disp_flg==1)
	  {
		restore_disp_flg=0;
		clear_disp_shan_flg();
		key_ext_flg=1;
		for(i=0;i<4;i++)
		{
			test=Ch2VoltLedBuf[i];
			WriteAll_1621Byte_CH2(i*2,&test,1);
		}
	  }				
                //ch1_cur_location=main_location_curr;
    	     }
    	  
    	  else if(ch2_cur_location==main_location_curr)
    	  {
               //ch1_cur_location=main_location_volt;
               addr=ch2_curr_fine_index*2+8;
               test=Ch2CurrLedBuf[ch2_curr_fine_index];
		if(ch2_curr_fine_index==2)
		{
	        if(g_stat.ALL_ON_FLG)
	         {
	            SET_BIT(test, LED_BIT3); 
		       // CLEAR_BIT(test, LED_BIT3); 
	            SET_BIT(clear, LED_BIT3); 
		       // CLEAR_BIT(clear, LED_BIT3); 
				
	         }
	        else
	        {
	           // SET_BIT(test, LED_BIT3); 
		        CLEAR_BIT(test, LED_BIT3); 
	           // SET_BIT(clear, LED_BIT3); 
		        CLEAR_BIT(clear, LED_BIT3); 
	        }
			Ch2CurrLedBuf[ch2_curr_fine_index]=test;
		}
		if(ch2_curr_fine_index==3)
		{
	        if(g_stat.ALL_ON_FLG)
	         {
	           // SET_BIT(test, LED_BIT3); 
		        CLEAR_BIT(test, LED_BIT3); 
	           // SET_BIT(clear, LED_BIT3); 
		        CLEAR_BIT(clear, LED_BIT3); 
				
	         }
	        else
	        {
	            SET_BIT(test, LED_BIT3); 
		       // CLEAR_BIT(test, LED_BIT3); 
	            SET_BIT(clear, LED_BIT3); 
		       // CLEAR_BIT(clear, LED_BIT3); 
	        }
			Ch2CurrLedBuf[ch2_curr_fine_index]=test;
		}
	  if(led_clear_flg==1&&led_disp_flg==0&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch2VoltledshanIn=1;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&clear,1);
	  }
	  	  if(led_clear_flg==0&&led_disp_flg==1&&led_reflesh==1)
	  {
	  	led_reflesh=0;
	  	ch2VoltledshanIn=0;
		key_ext_flg=1;
	    WriteAll_1621Byte_CH2(addr,&test,1);
	  }
	  if(restore_disp_flg==1)
	  {
	  	clear_disp_shan_flg();
		key_ext_flg=1;
		restore_disp_flg=0;
		for(i=0;i<4;i++)
		{
			test=Ch2CurrLedBuf[i];
			if(i==2||i==3)
			{
	if(g_stat.ALL_ON_FLG)
	{
	    SET_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
	}
	else
	{
	    SET_BIT(Ch2CurrLedBuf[3], LED_BIT3); 
		CLEAR_BIT(Ch2CurrLedBuf[2], LED_BIT3); 
	}
			}
test=Ch2CurrLedBuf[i];
key_ext_flg=1;
			WriteAll_1621Byte_CH2(i*2+8,&test,1);
		}
	  }
    	  }

    
		  

    }
  }
}


static void uctsk_Led (void) 
{     
	u8 test,addr;
	u8 clear=0;
   	for(;;)
   	{  
	  // usbtmc_scheduler();
	  if(cur_page==page_calibration)
	  {
	  	if(fine_set_flg==0&&cal_set_flg==1)
	  	{
            cal_set_flg=0;
			cal_set_reflesh_flg=1;
		}
            if(fine_set_flg==0)
            {
				cal_set_flg=0;
            }
			else
			{
                cal_set_flg=1;
			}
				
	  }
	  if(fine_set_flg)
	  {
			if(CHAN_STAT==CH1_SEL)
			{
				
				
					 OSTimeDlyHMSM(0, 0, 0, 250);	
					 if(fine_set_flg==0)
					   	continue;
                      // WriteAll_1621Byte_CH1(addr,&clear,1);
					   led_disp_flg=0;
					   led_clear_flg=1;
					   led_reflesh=1;
	                   OSTimeDlyHMSM(0, 0, 0, 250);	
					    if(fine_set_flg==0)
					   	continue;
					   led_disp_flg=1;
					   led_clear_flg=0;
					   led_reflesh=1;
	                  // WriteAll_1621Byte_CH1(addr,&test,1);
	                //   OSTimeDlyHMSM(0, 0, 0, 250);
			}
			else
			{
				 OSTimeDlyHMSM(0, 0, 0, 250);	
				 	if(fine_set_flg==0)
					   	continue;
                       		led_disp_flg=0;
					 led_clear_flg=1;
					 led_reflesh=1;
                     //  WriteAll_1621Byte_CH2(addr,&clear,1);
	                   OSTimeDlyHMSM(0, 0, 0, 250);	
				if(fine_set_flg==0)
					   	continue;
					   led_disp_flg=1;
					   led_clear_flg=0;
					   led_reflesh=1;
	                  // WriteAll_1621Byte_CH2(addr,&test,1);
	                 //  OSTimeDlyHMSM(0, 0, 0, 250);
			}
	  	


	  }
	  else
	  {
	  	               cal_cur_location=0; 
					   led_disp_flg=0;
					   led_clear_flg=0;
	  }
	  if(OTPMode_flg)
	  {
	  				
					   led_disp_flg1=0;
					 //  led_clear_flg=1;
	                   OSTimeDlyHMSM(0, 0, 0, 250);	
					   led_disp_flg1=1;
					    OSTimeDlyHMSM(0, 0, 0, 250);	
					//   led_clear_flg=0;
	                  // WriteAll_1621Byte_CH1(addr,&test,1);
	                   //OSTimeDlyHMSM(0, 0, 0, 250);
	  }
	   OSTimeDlyHMSM(0, 0, 0, 10);	 /* 20 MS  */
    }
}


void draw_usb(void)
{
	HT1621_set_bit(4,1,3);
}
void clear_usb(void)
{
 HT1621_clear_bit(4,1,3);
}
void clear_ch3_cc_cv(void)
{
 HT1621_clear_bit(26,1,0);
 HT1621_clear_bit(26,1,1);
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
