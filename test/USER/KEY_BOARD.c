#include <includes.h>            
#include "Main.h"
#include "common.h"
extern u8 lock_stat, fine_set_flg;
extern u8 Adc_get_clear;
extern u8 ch3_ChannelSel;
extern u8 BeeperFlg;
extern u8 led_clear_flg,led_disp_flg,led_disp_flg1;
extern u8 CH3Mode_flg,CH3Mode_reflesh;
extern u8  recall_flg_reflsh;

void deal_ALL_ON (void);
Cal_K Cal_Test(cal_data val);
void deal_store(void);
void deal_recall(void);
void deal_delete(void);
void deal_CH1 (u8 clear);
void deal_CH2 (u8 clear);
u16 set_store(STORE_PARA  para_store,uint32_t Address)
{
	u8 i;
	para_store.flg=0xaa;
	para_store.ch3_Sel=ch3_ChannelSel;
	para_store.ch1_volt_set=CHAN1.volt_set;
	para_store.ch2_volt_set=CHAN2.volt_set;
	para_store.ch1_curr_set=CHAN1.curr_set;
	para_store.ch2_curr_set=CHAN2.curr_set;
	para_store.ch3_volt_set=CHAN3.volt_set;
	for(i=0;i<5;i++)
	{

	}
	if(BeeperFlg)
	{
	 para_store.beeper_flg=1;
	}
	else
	{
	para_store.beeper_flg=0;
	}
	if(g_stat.SER_FLG||g_stat.PARA_FLG)
	{
		if(g_stat.PARA_FLG )
		{
			para_store.mode_flg=0x02;
		}
		else if(g_stat.SER_FLG)
		{
			para_store.mode_flg=0x03;
		}
	}
	else
		para_store.mode_flg=0x00;
	if(PARA_Saved(Address,&para_store, sizeof(para_store)))
		return 1;
	else
		return 0;
}


void Rcl_set(STORE_PARA  para_store)
{
	u8 i;
	char Result[10]={0};
	ch3_ChannelSel=para_store.ch3_Sel;
	CHAN1.volt_set=para_store.ch1_volt_set;
	if(CHAN1.volt_set<=0)
		CHAN1.volt_set=0;
	CHAN1.curr_set=para_store.ch1_curr_set;
	if(CHAN1.curr_set<=0)
		CHAN1.curr_set=0;
	floattostr(Result,CHAN1.volt_set);
	CHAN1.volt_set=atof(Result);
	floattostr(Result,CHAN1.curr_set);
	CHAN1.curr_set=atof(Result);
	for(i=0;i<5;i++)
	{

	}
	CHAN2.curr_set=para_store.ch2_curr_set;
	if(CHAN2.curr_set<=0)
		CHAN2.curr_set=0;
	CHAN2.volt_set=para_store.ch2_volt_set;
	if(CHAN2.volt_set<=0)
		CHAN2.volt_set=0;
	CHAN3.volt_set=para_store.ch3_volt_set;
	if(CHAN3.volt_set<=0)
		CHAN3.volt_set=0;

	floattostr(Result,CHAN2.volt_set);
	CHAN2.volt_set=atof(Result);
	floattostr(Result,CHAN2.curr_set);
	CHAN2.curr_set=atof(Result);

	floattostr(Result,CHAN3.volt_set);
	CHAN3.volt_set=atof(Result);

	BeeperFlg=para_store.beeper_flg;
	for(i=0;i<5;i++)
	{

	}

	//CHAN2.tmr_volt[3]=4;
}


void (*store_info_call[3])(void) =
{
	deal_store,
	deal_recall,
	deal_delete,
};

void deal_store(void)
{

}

void _recall(void)
	{
		Rcl_set(para_store);
		rcl_inprogess=1;
		if(para_store.mode_flg==0x02)
		{
			SCPI_PARA_FLG=1;
			SCPI_PARA_SET=1;
			key_vale=KEY_PARA;
			key_stat=1;
		}
		else if(para_store.mode_flg==0x03)
		{
			SCPI_SER_FLG=1;
			SCPI_SER_SET=1;
			key_vale=KEY_SER;
			key_stat=1;
		}
		else
		{
			if(g_stat.SER_FLG||g_stat.PARA_FLG)
			{
				if(g_stat.PARA_FLG )
				{
					SCPI_PARA_FLG=0;
					SCPI_PARA_SET=1;
					key_vale=KEY_PARA;
					key_stat=1;
				}
				else if(g_stat.SER_FLG)
				{
					SCPI_SER_FLG=0;
					SCPI_SER_SET=1;
					key_vale=KEY_SER;
					key_stat=1;
				}
			}
			else
			{
				g_stat.ALL_ON_FLG=0xff;
				key_vale=KEY_ALL_ON;
				key_stat=1;
				
				//CH1_SET_FLG=1;
				//CH2_SET_FLG=1;
			}
		}
		recall_SET_FLG=1;
		//CH1_SET_FLG=1;
		//CH2_SET_FLG=1;
		//HT1621_Disp_CH3_volt(CHAN3.volt_set);
		//SetReflsehAll();
		//SET_TIMER_REFLESH(CH1_SEL,&CHAN1);
		//SET_TIMER_REFLESH(CH2_SEL,&CHAN2);
	}


void deal_recall(void)
{

}


void deal_delete(void)
{

}


void left_deal(void)
{
	switch(cur_page)
	{
		case page_main:

			break;
		case page_timer: 

			break;
		case page_store: 
 
			break;
		default:
			break;
	}
}


void right_deal(void)
{
	switch(cur_page)
	{
		case page_main:

			break;
		case page_timer:

			break;
		case page_store: 

			break;
		default:
			break;
	}
}


void up_deal(void)
{
	
	switch(cur_page)
	{
		case page_main:
			if(fine_set_flg==0)
				break;
			if(lock_stat)
				break;
			 fine_set_flg=1;
			TIMER_CH1.sec_count=0;
			if(CHAN_STAT==CH2_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
			{	
				BEEP_RING(50);

				break;
			}

			if(main_cur_location==main_location_volt)
			{


				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;
						pt_PARA->volt_set =pt_PARA->volt_set+pt_PARA->volt_set_step;//+volt_step
					}
					if(pt_PARA->volt_set<10&&pt_PARA->volt_set>9.9995)
					{
						pt_PARA->volt_set=10;
					}
					if(pt_PARA->volt_set>Volt_max)
					{
						pt_PARA->volt_set=Volt_max;
						SET_REFLESH();
						BEEP_RING(50);
			
					}
					else
					{
						SET_REFLESH();
					}


					if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
						CHAN2.volt_set=CHAN1.volt_set;			
					
				}

			}
			else if(main_cur_location==main_location_curr)
			{

				{			
					key_vale=KEY_NONE;
					key_stat=1;
					pt_PARA->curr_set =pt_PARA->curr_set+pt_PARA->curr_set_step;//curr_step
					if(pt_PARA->curr_set>Curr_max)
					{
						pt_PARA->curr_set=Curr_max;
						SET_REFLESH();

						BEEP_RING(50);
						
					}
                  else
                  {
                  	SET_REFLESH();
                  }

					if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
						CHAN2.curr_set=CHAN1.curr_set;
					if(CHAN_STAT==CH1_SEL&&(g_stat.PARA_FLG))
					{
							//CHAN1.curr_set =CHAN1.curr_set+CHAN1.curr_set*0.018125;
							//CHAN2.curr_set= CHAN1.curr_set ;
							
					}
 
				}

			}


			break;
case page_ovp_ocp:
				if(fine_set_flg==0)
				break;
			if(lock_stat)
				break;
			 fine_set_flg=1;



			if(main_cur_location==main_location_volt)
			{


				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						pt_PARA->volt_ovp_set =pt_PARA->volt_ovp_set+pt_PARA->volt_set_step;//+volt_step
					}
					if(pt_PARA->volt_ovp_set<10&&pt_PARA->volt_ovp_set>9.9995)
					{
						pt_PARA->volt_ovp_set=10;
					}
					if(pt_PARA->volt_ovp_set>Volt_max)
					{
						pt_PARA->volt_ovp_set=Volt_max;
						SET_REFLESH_OVP_PAGE();
						BEEP_RING(50);
			
					}
					else
					{
						SET_REFLESH_OVP_PAGE();
					}
		
					
				}

			}
			else if(main_cur_location==main_location_curr)
			{

				{	
					key_vale=KEY_NONE;
					key_stat=1;

					pt_PARA->curr_ocp_set =pt_PARA->curr_ocp_set+pt_PARA->curr_set_step;//curr_step
					if(pt_PARA->curr_ocp_set>Curr_max)
					{
						pt_PARA->curr_ocp_set=Curr_max;
						SET_REFLESH_OVP_PAGE();

						BEEP_RING(50);
					
					}
                  else
                  {
                  	SET_REFLESH_OVP_PAGE();
                  }


 
				}

			}
		
			

			break;

	break;
case page_calibration:
			if(lock_stat)
				break;
			 fine_set_flg=1;
             TIMER_CH1.sec_count=0;
			 if(CHAN_SEL==CH3_SEL)
			 {
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						CHAN3.cal_volt_set =CHAN3.cal_volt_set+CHAN3.volt_set_step;//+volt_step
					}
					if(CHAN3.cal_volt_set<10&&CHAN3.cal_volt_set>9.9995)
					{
						CHAN3.cal_volt_set=10;
					}
					if(CHAN3.cal_volt_set>7)
					{
						CHAN3.cal_volt_set=7;
						BEEP_RING(50);
					}
					Cal_Volt_Disp(CHAN3.cal_volt_set,0,0);
				break;
			 }


			if(main_cur_location==main_location_volt)
			{

	
				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						pt_PARA->cal_volt_set =pt_PARA->cal_volt_set+pt_PARA->volt_set_step;//+volt_step
					}
					if(pt_PARA->cal_volt_set<10&&pt_PARA->cal_volt_set>9.9995)
					{
						pt_PARA->cal_volt_set=10;
					}
					if(pt_PARA->cal_volt_set>Volt_max_cal)
					{
						pt_PARA->cal_volt_set=Volt_max_cal;
						SET_REFLESH_CAL_PAGE();
						BEEP_RING(50);
						//deal_main_info(3);
					}
					else
					{
						SET_REFLESH_CAL_PAGE();
					}
		
					
				}

			}
			else if(main_cur_location==main_location_curr)
			{
	
				{	
					key_vale=KEY_NONE;
					key_stat=1;

					pt_PARA->cal_curr_set =pt_PARA->cal_curr_set+pt_PARA->curr_set_step;//curr_step
					if(pt_PARA->cal_curr_set>Curr_max_cal)
					{
						pt_PARA->cal_curr_set=Curr_max_cal;
						SET_REFLESH_CAL_PAGE();

						BEEP_RING(50);
				
					}
                  else
                  {
                  	SET_REFLESH_CAL_PAGE();
                  }

 
				}

			}
		
	
						

			break;

	break;
		default:
			break;
	}
}


void down_deal(void)
{
	switch(cur_page)
	{
		case page_main:
			if(fine_set_flg==0)
				break;
			if(lock_stat)
				break;
             fine_set_flg=1;
             TIMER_CH1.sec_count=0;
			if(CHAN_STAT==CH2_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
			{
				BEEP_RING(50);
				break;
			}

			if(main_cur_location==main_location_volt)
			{


				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						pt_PARA->volt_set =pt_PARA->volt_set-pt_PARA->volt_set_step;//-volt_step
					}
					if(pt_PARA->volt_set<10&&pt_PARA->volt_set>9.9995)
					{
						pt_PARA->volt_set=10;
					}
					if(pt_PARA->volt_set<0)
					{
						pt_PARA->volt_set=0;
						// warnning_flg=1;
                        SET_REFLESH();
						BEEP_RING(50);
						//deal_main_info(3);						
					}
					else
					{
						SET_REFLESH();
					}					


					if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
						CHAN2.volt_set=CHAN1.volt_set;

				}

			}
			else if(main_cur_location==main_location_curr)
			{
	
				{
					key_vale=KEY_NONE;
					key_stat=1;

					pt_PARA->curr_set =pt_PARA->curr_set-pt_PARA->curr_set_step;
					if(pt_PARA->curr_set<0)
					{
						pt_PARA->curr_set=0;
						SET_REFLESH();
						BEEP_RING(50);
					//	deal_main_info(4);
					}
                  else
                  {
                  	SET_REFLESH();
                  }	

					if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
						CHAN2.curr_set=CHAN1.curr_set;

				}

			}

			else
			main_cur_location=main_location_volt ;
			break;
		 case page_ovp_ocp:
			if(lock_stat)
				break;
			 fine_set_flg=1;
             TIMER_CH1.sec_count=0;
			if(CHAN_STAT==CH2_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
			{
				BEEP_RING(50);
				break;
			}

			if(main_cur_location==main_location_volt)
			{


				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						pt_PARA->volt_ovp_set =pt_PARA->volt_ovp_set-pt_PARA->volt_set_step;//-volt_step
					}
					if(pt_PARA->volt_ovp_set<10&&pt_PARA->volt_ovp_set>9.9995)
					{
						pt_PARA->volt_ovp_set=10;
					}
					if(pt_PARA->volt_ovp_set<0)
					{
						pt_PARA->volt_ovp_set=0;
						// warnning_flg=1;
                        SET_REFLESH_OVP_PAGE();
					    BEEP_RING(50);
						//deal_main_info(3);						
					}
					else
					{
						SET_REFLESH_OVP_PAGE();
					}					


					//if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
						//CHAN2.volt_set=CHAN1.volt_set;

				}

			}
			else if(main_cur_location==main_location_curr)
			{

				{
					key_vale=KEY_NONE;
					key_stat=1;

					pt_PARA->curr_ocp_set =pt_PARA->curr_ocp_set-pt_PARA->curr_set_step;
					if(pt_PARA->curr_ocp_set<0)
					{
						pt_PARA->curr_ocp_set=0;
						SET_REFLESH_OVP_PAGE();
						BEEP_RING(50);
					//	deal_main_info(4);
					}
                  else
                  {
                  	SET_REFLESH_OVP_PAGE();
                  }	

					//if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
					//	CHAN2.curr_set=CHAN1.curr_set;

				}

			}

			else
			main_cur_location=main_location_volt ;
			break;
			
			
			//TIMER_Set(timer_cur_location,1,0); 
			
			//TIMER_REFLESH();
			// Timer_Chart_Rflesh();
			break;
		case page_calibration:
			if(lock_stat)
				break;
			 fine_set_flg=1;
             TIMER_CH1.sec_count=0;
			 if(CHAN_SEL==CH3_SEL)
			 {
				 if(beep_on==0)
				 {
					 key_vale=KEY_NONE;
					 key_stat=1;

					 CHAN3.cal_volt_set =CHAN3.cal_volt_set-CHAN3.volt_set_step;//-volt_step
				 }
				 if(CHAN3.cal_volt_set<10&&CHAN3.cal_volt_set>9.9995)
				 {
					 CHAN3.cal_volt_set=10;
				 }
				 if(CHAN3.cal_volt_set<0)
				 {
					 CHAN3.cal_volt_set=0;
					 BEEP_RING(50);		 
				 }

					Cal_Volt_Disp(CHAN3.cal_volt_set,0,0);
				break;
			 }


			if(main_cur_location==main_location_volt)
			{


				{
					if(beep_on==0)
					{
						key_vale=KEY_NONE;
						key_stat=1;

						pt_PARA->cal_volt_set =pt_PARA->cal_volt_set-pt_PARA->volt_set_step;//-volt_step
					}
					if(pt_PARA->cal_volt_set<10&&pt_PARA->cal_volt_set>9.9995)
					{
						pt_PARA->cal_volt_set=10;
					}
					if(pt_PARA->cal_volt_set<0)
					{
						pt_PARA->cal_volt_set=0;
						// warnning_flg=1;
                        SET_REFLESH_CAL_PAGE();
					    BEEP_RING(50);
						//deal_main_info(3);						
					}
					else
					{
						SET_REFLESH_CAL_PAGE();
					}					




				}

			}
			else if(main_cur_location==main_location_curr)
			{

				{
					key_vale=KEY_NONE;
					key_stat=1;

					pt_PARA->cal_curr_set =pt_PARA->cal_curr_set-pt_PARA->curr_set_step;
					if(pt_PARA->cal_curr_set<0)
					{
						pt_PARA->cal_curr_set=0;
						SET_REFLESH_CAL_PAGE();
						BEEP_RING(50);
	
					}
                  else
                  {
                  	SET_REFLESH_CAL_PAGE();
                  }	



				}

			}

	
						else
			//main_cur_location=main_location_volt ;
			break;
			break;
		default:
			break;
	}
}


void ok_deal(void)
{	
	switch(cur_page)
	{
		case page_main:
			if(main_info_flg)
			{
			}
			else
			{
				if(main_cur_location==main_location_volt)
				{
					main_cur_location=main_location_curr;
					Volt_Cur_Change(pt_PARA,1);
					Curr_Cur_Change(pt_PARA,1);
				}
				else if(main_cur_location==main_location_curr)
				{

				}

				else
					main_cur_location=main_location_volt ;
			}
			break;

		case page_store: 

			break;
		default:
			break;
	}

}


void deal_SER (void)
{
	u16 temp;
	u16 tmp;


	CH1POWER_OFF;
	CH2POWER_OFF;
	if(g_stat.SER_FLG)
		temp=0;
	else
		temp=1;
	
	DRAW_CLEAR_SER(temp);
	if(g_stat.SER_FLG)
	{
		LED_ON_OFF(0);
		DELAY_SER_ON;
		CHAN2.volt_set=CHAN1.volt_set;
		CHAN2.curr_set=CHAN1.curr_set;
		CH1_SET_FLG=1;
		CH2_SET_FLG=1;
		CH1_STAT=0;
		CH2_STAT=0;
		deal_CH1(1);
		deal_CH2(1);
		g_stat.set_reflesh=1;

	}
	else
	{
		LED_ON_OFF(0);
		DELAY_SER_OFF;
		CH1_STAT=0;
		CH2_STAT=0;
		deal_CH1(1);
		deal_CH2(1);
		//deal_ALL_ON();
	}

 
}


void deal_PARA (void)
{
	u16 temp;
	u16 tmp;

	CH1POWER_OFF;
	CH2POWER_OFF;
	if(g_stat.PARA_FLG)
		temp=0;
	else
		temp=1;
	
	DRAW_CLEAR_PARA(temp);
	if(g_stat.PARA_FLG)
	{
		LED_ON_OFF(0);
		DELAY_PARA_ON;
		DELAY_PARA1_ON;
		DELAY_PARA2_ON;

		CHAN2.volt_set=CHAN1.volt_set;
		CHAN2.curr_set=CHAN1.curr_set;
		CH1_SET_FLG=1;
		CH2_SET_FLG=1;
		//VOLT_REFLESH=1;
		//CURR_REFLESH=1;
		//set_reflesh=1;
		//g_stat.ALL_ON_FLG=0xff;
		//deal_ALL_ON();
		CH1_STAT=0;
		CH2_STAT=0;
		deal_CH1(1);
		deal_CH2(1);
		g_stat.set_reflesh=1;
	}
	else
	{
		//g_stat.ALL_ON_FLG=0xff;

		DELAY_PARA_OFF;
		DELAY_PARA1_OFF;
		DELAY_PARA2_OFF;		
		CH1_SET_FLG=1;
		CH2_SET_FLG=1;
		CH1_STAT=0;
		CH2_STAT=0;
		LED_ON_OFF(0);
		deal_CH1(1);
		deal_CH2(1);
	}

}


void deal_CH3 (void)
{
}

extern u8 LoadFlg;
void deal_CH1 (u8 clear)
{
	u16 temp;
	CPU_SR         cpu_sr;

	if(CH1_STAT==0&&g_stat.ALL_ON_FLG)
	{
		g_stat.ALL_ON_FLG=0;

	}
	OS_ENTER_CRITICAL(); 
	if(CH1_STAT)
	{
		if(g_stat.PARA_FLG)
		{
			 //CH1POWER_OFF;
			// para_ch1_on_flg=0;
		}
		else
		{
		    // CH1POWER_ON;	
			// para_ch1_on_flg=1;
		}
		CH1POWER_ON;	
		CHAN1.STAT_flg=1;
		Adc_get_clear=1;
		CH1Mode_reflesh=1;


	}
	else
	{
		//disp_reflesh=0;
		LoadFlg=0;
		CH1POWER_OFF; 
		CHAN1.STAT_flg=0;


	}


	OS_EXIT_CRITICAL();
}


void deal_CH2 (u8 clear)
{
	u16 temp;
	CPU_SR         cpu_sr;

	if(CH2_STAT==0&&g_stat.ALL_ON_FLG)
	{
		g_stat.ALL_ON_FLG=0;

	}
	OS_ENTER_CRITICAL(); 
	if(CH2_STAT)
	{

		CH2POWER_ON; 
		CHAN2.STAT_flg=1;

		Adc_get_clear=1;
		CH2Mode_reflesh=1;




	}
	else
	{

		CH2POWER_OFF; 
		CHAN2.STAT_flg=0;




	}
	
	OS_EXIT_CRITICAL();
}
void deal_OCP(void)
{
	float temp;
		switch(cur_page)
		{
			case page_main:
				g_stat.OCP_ON_FLG=(~g_stat.OCP_ON_FLG);
				if(g_stat.OCP_ON_FLG)
				{
                    LED_OCP(1);
				}
				else
				{
                    LED_OCP(0);
				}
				//PAGE_MAIN();
				break;
			case page_ovp_ocp:
				//PAGE_OVP_MAIN();
				break;
			case page_calibration: 
				LED_OCP(1);
				DELAY_MS2(10);
				LED_OCP(0);
				BEEP_RING1( 10);
		if(CHAN_SEL==CH1_SEL)
    	{
             if(main_cur_location==main_location_volt)
               {
    		    temp=CHAN1.cal_volt_set;
    			Cal_Volt.Cal_X2=ch1_volt_disp_cal;
    			Cal_Volt.Cal_Y2=temp;
    			Cal_VoltSet.Cal_X2=temp;
    			Cal_VoltSet.Cal_Y2=CHAN1.volt_set;
    			PARA_K[CH1_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
    			PARA_K[CH1_VOLT_SET_CAL]=Cal_Test(Cal_VoltSet);
                }
    		 else if(main_cur_location==main_location_curr)
    		 {

    		 if(g_stat.PARA_FLG)
    		 {
			temp=CHAN1.cal_curr_set/2;
			Cal_CurrSet.Cal_X2=temp;
			Cal_CurrSet.Cal_Y2=CHAN1.curr_set;
	    		PARA_K[CH1_CURR_PARA_SET_CAL]=Cal_Test(Cal_CurrSet);
		}
		else
			{
    		 	temp=CHAN1.cal_curr_set;
    			Cal_CurrSet.Cal_X2=temp;
    			Cal_CurrSet.Cal_Y2=CHAN1.curr_set;
    			Cal_Curr.Cal_X2=ch1_curr_disp_cal;
    			Cal_Curr.Cal_Y2=temp;
	    				PARA_K[CH1_CURR_SET_CAL]=Cal_Test(Cal_CurrSet);
				PARA_K[CH1_CURR_DISP_CAL]=Cal_Test(Cal_Curr);
			}
				

    		 }

    		 
    	}
		else if(CHAN_SEL==CH2_SEL)
		{
             if(main_cur_location==main_location_volt)
               {
    		    temp=CHAN2.cal_volt_set;
    			Cal_Volt.Cal_X2=ch2_volt_disp_cal;
    			Cal_Volt.Cal_Y2=temp;
    			Cal_VoltSet.Cal_X2=temp;
    			Cal_VoltSet.Cal_Y2=CHAN2.volt_set;
    			PARA_K[CH2_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
    			PARA_K[CH2_VOLT_SET_CAL]=Cal_Test(Cal_VoltSet);
                }
    		 else if(main_cur_location==main_location_curr)
    		 {
			 if(g_stat.PARA_FLG)
			 {
				return;
			 }

    		 	temp=CHAN2.cal_curr_set;
    			Cal_CurrSet.Cal_X2=temp;
    			Cal_CurrSet.Cal_Y2=CHAN2.curr_set;

    			Cal_Curr.Cal_X2=ch2_curr_disp_cal;
    			Cal_Curr.Cal_Y2=temp;
    			PARA_K[CH2_CURR_SET_CAL]=Cal_Test(Cal_CurrSet);
				PARA_K[CH2_CURR_DISP_CAL]=Cal_Test(Cal_Curr);

    		 }
    	
    		 
		}
else
{
	temp=CHAN3.cal_volt_set;
	Cal_Volt.Cal_X2=ch3_volt_disp_cal;
	Cal_Volt.Cal_Y2=temp;
	Cal_VoltSet.Cal_X2=temp;
	Cal_VoltSet.Cal_Y2=CHAN3.volt_set;
	PARA_K[CH3_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
	PARA_K[CH3_VOLT_SET_CAL]=Cal_Test(Cal_VoltSet);

}
				break;
			case page_store: 
				break;
			case page_version:
			// PAGE_VERSION();
				break;
			default:
				break;
		}
}
void deal_OVP(void)
{
	float temp;
		switch(cur_page)
		{
			case page_main:
				g_stat.OVP_ON_FLG=(~g_stat.OVP_ON_FLG);
				if(g_stat.OVP_ON_FLG)
				{
                    LED_OVP(1);
				}
				else
				{
                    LED_OVP(0);
				}
				//PAGE_MAIN();
				break;
			case page_ovp_ocp:

				//PAGE_OVP_MAIN();
				break;
			case page_calibration: 
				LED_OVP(1);
				DELAY_MS2(10);
				LED_OVP(0);
				BEEP_RING1( 10);
		if(CHAN_SEL==CH1_SEL)
    	{
             if(main_cur_location==main_location_volt)
               {
    		        temp=CHAN1.cal_volt_set;
    			    Cal_Volt.Cal_X1=ch1_volt_disp_cal;
    			    Cal_Volt.Cal_Y1=temp;
    			    Cal_VoltSet.Cal_X1=temp;
    			    Cal_VoltSet.Cal_Y1=CHAN1.volt_set;
                }
    		 else if(main_cur_location==main_location_curr)
    		 {
    		 if(g_stat.PARA_FLG)
    		 {
			temp=CHAN1.cal_curr_set/2;
			Cal_CurrSet.Cal_X1=temp;
			Cal_CurrSet.Cal_Y1=CHAN1.curr_set;
		}
		else 
		{
    		 	temp=CHAN1.cal_curr_set;
    			Cal_CurrSet.Cal_X1=temp;
    			Cal_CurrSet.Cal_Y1=CHAN1.curr_set;

				Cal_Curr.Cal_X1=ch1_curr_disp_cal;
    			Cal_Curr.Cal_Y1=temp;
		}
				
    		 }
    	
    		 
    	}
		else if(CHAN_SEL==CH2_SEL)
		{
             if(main_cur_location==main_location_volt)
               {
    		        temp=CHAN2.cal_volt_set;
    			    Cal_Volt.Cal_X1=ch2_volt_disp_cal;
    			    Cal_Volt.Cal_Y1=temp;
    			    Cal_VoltSet.Cal_X1=temp;
    			    Cal_VoltSet.Cal_Y1=CHAN2.volt_set;
                }
    		 else if(main_cur_location==main_location_curr)
    		 {
    		 	if(g_stat.PARA_FLG)
			 {
				return;
			 }
    		 	temp=CHAN2.cal_curr_set;
    			Cal_CurrSet.Cal_X1=temp;
    			Cal_CurrSet.Cal_Y1=CHAN2.curr_set;

    		 	Cal_Curr.Cal_X1=ch2_curr_disp_cal;
    			Cal_Curr.Cal_Y1=temp;	
    		 }
  
    		 
		}
		else
		{
			temp=CHAN3.cal_volt_set;
			Cal_Volt.Cal_X1=ch3_volt_disp_cal;
			Cal_Volt.Cal_Y1=temp;
			Cal_VoltSet.Cal_X1=temp;
			Cal_VoltSet.Cal_Y1=CHAN3.volt_set;

		}
				break;
			case page_store: 
				break;
			case page_version:
			// PAGE_VERSION();
				break;
			default:
				break;
		}
}
extern u8 ch1OVPLedBuf,ch2OVPLedBuf,beepLedbuf;
extern int ovp_count;
extern u8  ovp_flg;
extern u8  ovp_flash_flg;
void OutPutOff(void)
{
	fine_set_flg=0;
	ovp_flash_flg=1;
	ovp_flg=0;
	ovp_count=0;
	g_stat.ALL_ON_FLG=0;
	enter_main_page();
		CH1_STAT=0;
		CH2_STAT=0;
		CH1POWER_OFF; 
		CH2POWER_OFF; 
	//	Select_CH3(4);
		g_stat.ch3_stat=0;

		g_stat.set_reflesh=1;
		LED_ON_OFF(0);
		LED_CH1_CV_OFF ;
		LED_CH2_CV_OFF;
		BEEP_RING(50);

		CLEAR_BIT(ch2OVPLedBuf, LED_BIT2); 
		CLEAR_BIT(ch2OVPLedBuf, LED_BIT3); 
		WriteAll_1621_CH1(30,&ch2OVPLedBuf,1);

		CLEAR_BIT(ch1OVPLedBuf, LED_BIT2); 
		CLEAR_BIT(ch1OVPLedBuf, LED_BIT3); 	
		WriteAll_1621_CH1(8,&ch1OVPLedBuf,1);
		HT1621_Disp_ON_FLG(0);
		deal_CH1 (1);
	    deal_CH2 (1);	
		ch1_volt_disp=0;
		ch2_volt_disp=0;
		disp_reflesh=0;
					 	//	if(fine_set_flg)
					 		{
					 		
					 			fine_set_flg=0;
								led_disp_flg=0;
					  			 led_clear_flg=0;
					 			SET_REFLESH_SET_PAGE(main_cur_location);
								 led_clear_flg=0;
					 	
					 		}		
}
extern u8 adc_get_flg;
extern u8 ch1OVPLedBuf,ch2OVPLedBuf,beepLedbuf;
void deal_ALL_ON (void)
{
	g_stat.ALL_ON_FLG=(~g_stat.ALL_ON_FLG);
	enter_main_page();
	if(g_stat.ALL_ON_FLG)
	{

		
		CH2POWER_ON;

		if(g_stat.PARA_FLG)
		{
			 //CH1POWER_OFF;
			// para_ch1_on_flg=0;
		}
		else
		{
		    // CH1POWER_ON;	
			// para_ch1_on_flg=1;
		}
		CH1POWER_ON;
		ch1_volt_disp=0;
		ch2_volt_disp=0;
		disp_reflesh=0;
			//Select_CH3(ch3_ChannelSel);
			g_stat.ch3_stat=0xff;

			LED_ON_OFF(1);
			HT1621_Disp_ON_FLG(1);
			//DELAY_MS1(15);
			adc_get_flg=1;
			CH1_STAT=0xff;
		  	CH2_STAT=0xff;
			LED_SETVIEW(0);
			CH3Mode_reflesh=1;

	}
	else
	{
		LED_CH1_CV_OFF ;
		LED_CH2_CV_OFF;
		CH1_STAT=0;
		CH2_STAT=0;
		CH1POWER_OFF; 
		CH2POWER_OFF; 
		CH3_SET_FLG=1;
		CH1_SET_FLG=1;
		CH2_SET_FLG=1;
		recall_flg_reflsh=1;
		CLEAR_BIT(ch2OVPLedBuf, LED_BIT2); 
		CLEAR_BIT(ch2OVPLedBuf, LED_BIT3); 
		WriteAll_1621_CH1(30,&ch2OVPLedBuf,1);

		CLEAR_BIT(ch1OVPLedBuf, LED_BIT2); 
		CLEAR_BIT(ch1OVPLedBuf, LED_BIT3); 	
		WriteAll_1621_CH1(8,&ch1OVPLedBuf,1);
		clear_ch3_cc_cv();
		init_advalue();
		{
		g_stat.ch3_stat=0;

		g_stat.set_reflesh=1;
		LED_ON_OFF(0);
		HT1621_Disp_ON_FLG(0);

		}
		ch1_volt_disp=0;
		ch2_volt_disp=0;
		LED_SETVIEW(0);

	}
	deal_CH1 (1);
	deal_CH2 (1);
}


void deal_WAVEDISP (u8 chanel,P_SET_PARA pt_PARA,u8 flg)
{

}
/*
void  CalParam_Store( void )
{
	//PARA_K[8].a=0x55;
	//PARA_K[8].b=0x55;
	if(PARA_Saved(CAL_ADDRESS,&PARA_K[0], sizeof(PARA_K)))

	{

	}
}*/
void CalParam_deal(void)
{
	u8 i;
	for(i=0;i<12;i++)
	{
		if(PARA_K[i].a>10||PARA_K[i].a<=0)
		{
			PARA_K[i].a=1;
			PARA_K[i].b=0;
		}
	}
}













 
