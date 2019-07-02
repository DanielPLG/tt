#define	 MAINPAGE_GLOBALS

#include "Main.h"
#include "mainpage.h"
#include "timer.h"
extern int ovp_count;
extern u8  ovp_flg;
extern u8  ovp_flash_flg;
extern u8 adc_get_flg,CH2CV_mode_enable;
extern unsigned char adc_in_flg;
extern u8 lock_stat, fine_set_flg;

KEY_STAT_FLG g_stat;
void HC595_Write(u8 byte);

void HT1621_Disp_Cal_volt(unsigned char ch,double val);
void HT1621_Cal_curret_set(unsigned char ch,double val);
void HT1621_Disp_Cal_curret(unsigned char ch,float val);
extern char *s_SofewareVersion;
extern char *s_HardwareVersion;
void PAGE_VERSION(void)
{

}
 void Cal_Volt_Disp(double value,u16 offset,u8 mode)
{
	if(mode)
	{

	}
	else
	{

	}
	if(value<10)
	{

	}
	else
	{
	
	}
	//value=1;
    HT1621_Disp_Cal_volt(offset,value);
}
static void Cal_Curr_Disp(double value,u16 offset,u8 mode)
{
	if(mode)
	{

	}
	else
	{

	}
//value=0.5;
HT1621_Cal_curret_set(offset,value);
}

static void Volt_Disp(float value,u16 offset,u8 mode)
{
	if(mode)
	{

	}
	else
	{

	}
	if(value<10)
	{

	}
	else
	{
	
	}
	//value=1;
    HT1621_Disp_volt(offset,value);
}


static void Curr_Disp(float value,u16 offset,u8 mode)
{
	if(mode)
	{

	}
	else
	{

	}
//value=0.5;
HT1621_Disp_curret(offset,value);
}


void Volt_Cur_Change(P_SET_PARA pt_PARA,u8 flg)
{
	 int offset;
	 float Volt_temp;
	 Volt_temp=pt_PARA->volt_set;
	 if(pt_PARA==&CHAN1)
	{
		offset=0;
	}
	else
	{
		offset=1;
	}
	if(CH1_STAT&&pt_PARA==&CHAN1)
	{

		Volt_temp=ch1_volt_disp;
 
	}
	else if(CH2_STAT&&pt_PARA==&CHAN2)
	{

		if(g_stat.PARA_FLG )
			Volt_temp=ch1_volt_disp;	
		else
			Volt_temp=ch2_volt_disp;
			

	}
	if(pt_PARA->DISP_MODE)
	{
		if(main_cur_location==main_location_volt&&((pt_PARA==&CHAN1&&CHAN_STAT==CH1_SEL)||(pt_PARA==&CHAN2&&CHAN_STAT==CH2_SEL)))
		{
			if(flg)
			{

			}

		}
		else
		{
			if(flg)
			{

			}

		}
		// Volt_Disp(Volt_temp,offset,pt_PARA->DISP_MODE);
	}
	else
	{
		if(main_cur_location==main_location_volt&&((pt_PARA==&CHAN1&&CHAN_STAT==CH1_SEL)||(pt_PARA==&CHAN2&&CHAN_STAT==CH2_SEL)))
		{
			if(flg)
			{

			}

		}
		else
		{
			 if(flg)
			{

			}

		}
	}
	//Volt_temp=1;
	Volt_Disp(Volt_temp,offset,pt_PARA->DISP_MODE);
}
void Timer_Cur_Change(P_SET_PARA pt_PARA,u8 flg)
{
	int offset;

	if(pt_PARA==&CHAN1)
	{
		offset=0;
	}
	else
	{
		offset=1;
	}

}
void Curr_Cur_Change(P_SET_PARA pt_PARA,u8 flg)
{
	int offset;
	float Curr_temp;
	Curr_temp=pt_PARA->curr_set;
	if(pt_PARA==&CHAN1)
		offset=0;
	else
		offset=1;
	
	if(CH1_STAT&&pt_PARA==&CHAN1)
	{

		Curr_temp=ch1_curr_disp ;

	}
	else if(CH2_STAT&&pt_PARA==&CHAN2)
	{

	   if(g_stat.PARA_FLG||g_stat.SER_FLG )
	   		Curr_temp=ch1_curr_disp ;
		else
			Curr_temp=ch2_curr_disp ;

	}
	if(pt_PARA->DISP_MODE)
	{
		if(main_cur_location==main_location_curr&&((pt_PARA==&CHAN1&&CHAN_STAT==CH1_SEL)||(pt_PARA==&CHAN2&&CHAN_STAT==CH2_SEL)))
		{
			if(flg)
			{

			}

		}
		else
		{
			if(flg)
			{

			}
  
		}
	}
	else
	{
		if(main_cur_location==main_location_curr&&((pt_PARA==&CHAN1&&CHAN_STAT==CH1_SEL)||(pt_PARA==&CHAN2&&CHAN_STAT==CH2_SEL)))
		{
			if(flg)
			{

			}

		}
		else
		{
			if(flg)
			{

			}
  
		}
	}

	Curr_Disp(Curr_temp,offset,pt_PARA->DISP_MODE);
}

void Main_Timer_Reflesh(u8 flg)
{

}





void Volt_Cur_Set(void)
{
	int offset;
	float Volt_temp,curr_temp;
	Volt_temp=pt_PARA->volt_set;
	curr_temp=pt_PARA->curr_set;
	if(CHAN_STAT==CH1_SEL)
	{
		offset=0;
	}
	else
	{
		offset=1;
	}

	
	if(CH1_STAT&&CHAN_STAT==CH1_SEL)
	{

		Volt_temp=ch1_volt_disp;
		curr_temp=ch1_curr_disp;
		
	}
	else if(CH2_STAT&&CHAN_STAT==CH2_SEL)
	{

		if(g_stat.PARA_FLG)
			Volt_temp=ch1_volt_disp;	
		else
			Volt_temp=ch2_volt_disp;
		if(g_stat.PARA_FLG||g_stat.SER_FLG )
			curr_temp=ch1_curr_disp;
		else
			curr_temp=ch2_curr_disp;

	}

	Volt_Disp(Volt_temp,offset,pt_PARA->DISP_MODE);
	if(pt_PARA->DISP_MODE)
	{

	}
}

void SetReflsehAll(void)
{
	if(cur_page==page_main)
	{
        Volt_Disp(CHAN1.volt_set,0,0);
	    Volt_Disp(CHAN2.volt_set,1,0);
        Curr_Disp(CHAN1.curr_set,0,0);
		Curr_Disp(CHAN2.curr_set,1,0);
		DISP_POWER(CHAN1.Pow_set,0);
		DISP_POWER(CHAN2.Pow_set,1);
		HT1621_Disp_CH3_volt(CHAN3.volt_set);
	}
	else if(cur_page==page_ovp_ocp)
	{
        Volt_Disp(CHAN1.volt_ovp_set,0,0);
	    Volt_Disp(CHAN2.volt_ovp_set,1,0);
        Curr_Disp(CHAN1.curr_ocp_set,0,0);
		Curr_Disp(CHAN2.curr_ocp_set,1,0);
		DISP_POWER(CHAN1.power_ocp_set,0);
		DISP_POWER(CHAN2.power_ocp_set,1);
	}
	else if(cur_page==page_calibration)
	{
		if(CHAN_SEL==CH1_SEL)
		{
        Cal_Volt_Disp(CHAN1.cal_volt_set,0,0);
	   // Volt_Disp(CHAN2.cal_volt_set,1,0);
        Cal_Curr_Disp(CHAN1.cal_curr_set,0,0);
		//Curr_Disp(CHAN2.cal_curr_set,1,0);
		DISP_CURRENT_CAL_DISP(CHAN1.cal_curr_disp,0);
		}
		else if(CHAN_SEL==CH2_SEL)
		{
        Cal_Volt_Disp(CHAN2.cal_volt_set,0,0);
        Cal_Curr_Disp(CHAN2.cal_curr_set,0,0);
		DISP_CURRENT_CAL_DISP(CHAN2.cal_curr_disp,0);
		}
else
	{
	Cal_Volt_Disp(CHAN3.cal_volt_set,0,0);
	Cal_Curr_Disp(0,0,0);
	DISP_CURRENT_CAL_DISP(0,0);

	 HT1621_Disp_CH3_volt(CHAN3.volt_set);
	}

	}
}

void SET_REFLESH_SET_PAGE(u8 index)
{
	int offset;
	if(CHAN_STAT==CH1_SEL)
		offset=0;
	else
		offset=1;
	
	//set_reflesh=0;
	if(CHAN_SEL==CH3_SEL)
	{
     Cal_Volt_Disp(CHAN3.cal_volt_set,0,0);	
	 return;
	}

	if(index==main_location_volt)
	{
        if(cur_page==page_main)
        {

		Volt_Disp(pt_PARA->volt_set,offset,pt_PARA->DISP_MODE);
        }
		else if(cur_page==page_ovp_ocp)
		{
            Volt_Disp(pt_PARA->volt_ovp_set,offset,pt_PARA->DISP_MODE);
		}
		else if(cur_page==page_calibration)
		{
            //Volt_Disp(pt_PARA->cal_volt_set,offset,pt_PARA->DISP_MODE);
		 Cal_Volt_Disp(pt_PARA->cal_volt_set,0,pt_PARA->DISP_MODE);	
		}		


	} 
	   
	else if(index==main_location_curr)
	{

        if(cur_page==page_main)
        {

		Curr_Disp(pt_PARA->curr_set,offset,pt_PARA->DISP_MODE);
        }
		else if(cur_page==page_ovp_ocp)
		{
            Curr_Disp(pt_PARA->curr_ocp_set,offset,pt_PARA->DISP_MODE);
		}
		else if(cur_page==page_calibration)
		{
          //  Curr_Disp(pt_PARA->cal_curr_set,offset,pt_PARA->DISP_MODE);
		Cal_Curr_Disp(pt_PARA->cal_curr_set,0,pt_PARA->DISP_MODE);	
		}	

		//Curr_Disp(pt_PARA->curr_set,offset,pt_PARA->DISP_MODE);

		

	}

	
}
void SET_REFLESH_CAL_PAGE(void)
{
	int offset;
	if(CHAN_STAT==CH1_SEL)
		offset=0;
	else
		offset=1;
	
	set_reflesh=0;
	// VOLT_REFLESH=1;
/*	if(main_cur_location==main_location_volt)
		VOLT_REFLESH=1;
	else
		CURR_REFLESH=1;	
	

	if(g_stat.PARA_FLG||g_stat.SER_FLG)
	{
		VOLT_REFLESH=1;
		CURR_REFLESH=1;
	} */

	if(main_cur_location==main_location_volt)
	{
		/*if(CHAN_STAT==CH1_SEL&&CH1_STAT)
		{
			ch1_volt_reflesh=1;
			ch1_volt_cnt=0;
	
			if(g_stat.PARA_FLG||g_stat.SER_FLG )
			{
				ch2_volt_reflesh=1;
				ch2_volt_cnt=0;
			}

		}
		else if(CHAN_STAT==CH2_SEL&&CH2_STAT)
		{
			ch2_volt_reflesh=1;
			ch2_volt_cnt=0;

		}*/

		//Volt_Disp(pt_PARA->cal_volt_set,offset,pt_PARA->DISP_MODE);
		Cal_Volt_Disp(pt_PARA->cal_volt_set,0,0);
		/*if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->power_ocp_set=pt_PARA->curr_ocp_set*pt_PARA->volt_ovp_set; 
			DISP_POWER(pt_PARA->power_ocp_set,offset); 
		}

		if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
		{
			CHAN2.volt_ovp_set=CHAN1.volt_ovp_set;

			Volt_Disp(CHAN2.volt_ovp_set,1,CHAN2.DISP_MODE);
			if(CHAN2.DISP_MODE==0)
			{
				CHAN2.power_ocp_set=CHAN2.curr_ocp_set*CHAN2.volt_ovp_set;  
				DISP_POWER(CHAN2.power_ocp_set,1); 
			}
		}*/

	} 
	   
	else if(main_cur_location==main_location_curr)
	{



		//Curr_Disp(pt_PARA->cal_curr_set,offset,pt_PARA->DISP_MODE);
		Cal_Curr_Disp(pt_PARA->cal_curr_set,0,0);
		/*if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->power_ocp_set=pt_PARA->curr_ocp_set*pt_PARA->volt_ovp_set;   
			DISP_POWER(pt_PARA->power_ocp_set,offset);  
		}*/
		

	}

	
}
void SET_REFLESH_OVP_PAGE(void)
{
	int offset;
	if(CHAN_STAT==CH1_SEL)
		offset=0;
	else
		offset=1;
	
	set_reflesh=0;
	// VOLT_REFLESH=1;
	/*if(main_cur_location==main_location_volt)
		VOLT_REFLESH=1;
	else
		CURR_REFLESH=1;	
	

	if(g_stat.PARA_FLG||g_stat.SER_FLG)
	{
		VOLT_REFLESH=1;
		CURR_REFLESH=1;
	} */

	if(main_cur_location==main_location_volt)
	{
		/*if(CHAN_STAT==CH1_SEL&&CH1_STAT)
		{
			ch1_volt_reflesh=1;
			ch1_volt_cnt=0;
	
			if(g_stat.PARA_FLG||g_stat.SER_FLG )
			{
				ch2_volt_reflesh=1;
				ch2_volt_cnt=0;
			}

		}
		else if(CHAN_STAT==CH2_SEL&&CH2_STAT)
		{
			ch2_volt_reflesh=1;
			ch2_volt_cnt=0;

		}*/

		Volt_Disp(pt_PARA->volt_ovp_set,offset,pt_PARA->DISP_MODE);
		if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->power_ocp_set=pt_PARA->curr_ocp_set*pt_PARA->volt_ovp_set; 
			DISP_POWER(pt_PARA->power_ocp_set,offset); 
		}

		if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
		{
			CHAN2.volt_ovp_set=CHAN1.volt_ovp_set;

			Volt_Disp(CHAN2.volt_ovp_set,1,CHAN2.DISP_MODE);
			if(CHAN2.DISP_MODE==0)
			{
				CHAN2.power_ocp_set=CHAN2.curr_ocp_set*CHAN2.volt_ovp_set;  
				DISP_POWER(CHAN2.power_ocp_set,1); 
			}
		}

	} 
	   
	if(main_cur_location==main_location_curr)
	{
		if(CHAN_STAT==CH1_SEL&&CH1_STAT)
		{
			ch1_curr_reflesh=1;
			ch1_curr_cnt=0;
	
			if(g_stat.PARA_FLG||g_stat.SER_FLG )
			{
				ch2_curr_reflesh=1;
				ch2_curr_cnt=0;
			}

		}
		else if(CHAN_STAT==CH2_SEL&&CH2_STAT)
		{
			ch2_curr_reflesh=1;
			ch2_curr_cnt=0;

		}


		Curr_Disp(pt_PARA->curr_ocp_set,offset,pt_PARA->DISP_MODE);
		if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->power_ocp_set=pt_PARA->curr_ocp_set*pt_PARA->volt_ovp_set;   
			DISP_POWER(pt_PARA->power_ocp_set,offset);  
		}

		if(CHAN_STAT==CH1_SEL&&(g_stat.PARA_FLG||g_stat.SER_FLG ))
		{
			CHAN2.curr_ocp_set=CHAN1.curr_ocp_set;

			Curr_Disp(CHAN2.curr_ocp_set,1,CHAN2.DISP_MODE);
			if(CHAN2.DISP_MODE==0)
			{
				CHAN2.power_ocp_set=CHAN2.curr_ocp_set*CHAN2.volt_ovp_set;  
				DISP_POWER(CHAN2.power_ocp_set,1); 
			}
		 }

	}
}
void SET_REFLESH(void)
{
	int offset;
	if(CHAN_STAT==CH1_SEL)
		offset=0;
	else
		offset=1;
	
	set_reflesh=0;
	// VOLT_REFLESH=1;
	if(main_cur_location==main_location_volt)
		VOLT_REFLESH=1;
	else
		CURR_REFLESH=1;	
	

	if(g_stat.PARA_FLG||g_stat.SER_FLG)
	{
		CH1_SET_FLG=1;
		CH2_SET_FLG=1;
		VOLT_REFLESH=0;
		CURR_REFLESH=0;
	} 

	if(main_cur_location==main_location_volt)
	{
		if(CHAN_STAT==CH1_SEL&&CH1_STAT)
		{
			ch1_volt_reflesh=1;
			ch1_volt_cnt=0;
	
			if(g_stat.PARA_FLG||g_stat.SER_FLG )
			{
				ch2_volt_reflesh=1;
				ch2_volt_cnt=0;
			}

		}
		else if(CHAN_STAT==CH2_SEL&&CH2_STAT)
		{
			ch2_volt_reflesh=1;
			ch2_volt_cnt=0;

		}

		Volt_Disp(pt_PARA->volt_set,offset,pt_PARA->DISP_MODE);
		if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->Pow_set=pt_PARA->curr_set*pt_PARA->volt_set; 
			if(g_stat.ALL_ON_FLG==0)
			DISP_POWER(pt_PARA->Pow_set,offset); 
		}

		if(CHAN_STAT==CH1_SEL&&(g_stat.SER_FLG||g_stat.PARA_FLG))
		{
			CHAN2.volt_set=CHAN1.volt_set;

			Volt_Disp(CHAN2.volt_set,1,CHAN2.DISP_MODE);
			if(CHAN2.DISP_MODE==0)
			{
				CHAN2.Pow_set=CHAN2.curr_set*CHAN2.volt_set;  
				if(g_stat.ALL_ON_FLG==0)
				DISP_POWER(CHAN2.Pow_set,1); 
			}
		}

	} 
	   
	if(main_cur_location==main_location_curr)
	{
		if(CHAN_STAT==CH1_SEL&&CH1_STAT)
		{
			ch1_curr_reflesh=1;
			ch1_curr_cnt=0;
	
			if(g_stat.PARA_FLG||g_stat.SER_FLG )
			{
				ch2_curr_reflesh=1;
				ch2_curr_cnt=0;
			}

		}
		else if(CHAN_STAT==CH2_SEL&&CH2_STAT)
		{
			ch2_curr_reflesh=1;
			ch2_curr_cnt=0;

		}


		Curr_Disp(pt_PARA->curr_set,offset,pt_PARA->DISP_MODE);
		if(pt_PARA->DISP_MODE==0)
		{
			pt_PARA->Pow_set=pt_PARA->curr_set*pt_PARA->volt_set;
			if(g_stat.ALL_ON_FLG==0)
			DISP_POWER(pt_PARA->Pow_set,offset);  
		}

		if(CHAN_STAT==CH1_SEL&&(g_stat.PARA_FLG||g_stat.SER_FLG ))
		{
			CHAN2.curr_set=CHAN1.curr_set;

			Curr_Disp(CHAN2.curr_set,1,CHAN2.DISP_MODE);
			if(CHAN2.DISP_MODE==0)
			{
				CHAN2.Pow_set=CHAN2.curr_set*CHAN2.volt_set;  
				if(g_stat.ALL_ON_FLG==0)
				DISP_POWER(CHAN2.Pow_set,1); 
			}
		 }

	}
}


void MAIN_TIMER_SET(u8 flg,P_SET_PARA pt_PARA)
{
	u16 temp;

	if(flg)
	{

	}
	else 
	{

	}

}
static void _DrawPolygon(u8 channel,P_SET_PARA pt_PARA) 
{



}

void PAGE_TEST(void)
{

#ifdef CH1_RIGHT
	Channel_Sel(2)	;
#else
	Channel_Sel(1)	;
#endif
}  
void PAGE_MAIN(void)
{
	CPU_SR         cpu_sr;
	u16 temp;
	u8 i;
	float Volt_temp1,Volt_temp2,current_temp1,current_temp2;
	
	if(g_stat.set_reflesh)
	{
		g_stat.set_reflesh=0;
	    SetReflsehAll();
	}
	if(set_reflesh&&cur_page==page_main&&main_info_flg==0)
	{
		SET_REFLESH();
	}
	if(disp_reflesh&&g_stat.ALL_ON_FLG)
	{
		if(g_stat.OVP_ON_FLG)
		{
            		Volt_temp1=ch1_volt_disp;
			Volt_temp2=ch2_volt_disp;
			if(g_stat.PARA_FLG)
			{
				Volt_temp1=ch1_volt_disp;
				Volt_temp2=ch1_volt_disp;
			}
			else if(g_stat.SER_FLG)
			{

			}
			if(Volt_temp1>CHAN1.volt_ovp_set||Volt_temp2>CHAN2.volt_ovp_set)
			{
			if(Volt_temp1>CHAN1.volt_ovp_set)
				{
				draw_ovp_flg(1,0);
				}
			if(Volt_temp2>CHAN2.volt_ovp_set)
				{
				draw_ovp_flg(1,1);
				}
                OutPutOff();
			}
			
		}
		if(g_stat.OCP_ON_FLG)
		{
			current_temp1=ch1_curr_disp;
			current_temp2=ch2_curr_disp;
			if(g_stat.PARA_FLG)
			{
				current_temp1=para_curr_disp;
				current_temp2=para_curr_disp;
			}
			else if(g_stat.SER_FLG)
			{
				current_temp1=ch1_curr_disp;
				current_temp1=ch1_curr_disp;
			}
			if(current_temp1>CHAN1.curr_ocp_set||current_temp2>CHAN2.curr_ocp_set)
			{
			if(current_temp1>CHAN1.curr_ocp_set)
				{
				draw_ocp_flg(1,0);
				}
			if(current_temp2>CHAN2.curr_ocp_set)
				{
				draw_ocp_flg(1,1);
				}
               		 OutPutOff();
			}
		}
	}	
	if(disp_reflesh&&adc_in_flg==0&&cur_page==page_main&&g_stat.SetView_FLG==0)
	{
		disp_reflesh=0;
		if(CH1_STAT||CH2_STAT)
		{	
			//CH3_Measure();
			Main_Disp_reflesh();
			disp_reflesh=0;
		}

	}


}


void PAGE_OVP_MAIN(void)
{
	CPU_SR         cpu_sr;
	u16 temp;
	u8 i;
	float Volt_temp1,Volt_temp2,current_temp1,current_temp2,ad_value;
	if(g_stat.ovp_set_reflesh)
	{
		g_stat.ovp_set_reflesh=0;
	    SetReflsehAll();
	}	
	if(CH1_STAT)
		{
			ad_value=ch3_volt_disp;
			if(fine_set_flg&&CHAN_SEL==CH3_SEL)
			{
				ad_value=CHAN3.volt_set;
			}
			disp_ch3_volt_Read(ad_value);
		}
	

	if(disp_reflesh&&g_stat.ALL_ON_FLG)
	{
		if(g_stat.OVP_ON_FLG)
		{
            		Volt_temp1=ch1_volt_disp;
			Volt_temp2=ch2_volt_disp;
			if(g_stat.PARA_FLG)
			{
				Volt_temp1=ch1_volt_disp;
				Volt_temp2=ch1_volt_disp;
			}
			else if(g_stat.SER_FLG)
			{

			}
            //Volt_temp=ch1_volt_disp;
			if(Volt_temp1>CHAN1.volt_ovp_set||Volt_temp2>CHAN2.volt_ovp_set)
			{
			OutPutOff();
			if(Volt_temp1>CHAN1.volt_ovp_set)
				{
				draw_ovp_flg(1,0);
				}
			if(Volt_temp2>CHAN2.volt_ovp_set)
				{
				draw_ovp_flg(1,1);
				}
               	 
			}
			
		}
		if(g_stat.OCP_ON_FLG)
		{
			current_temp1=ch1_curr_disp;
			current_temp2=ch2_curr_disp;
			if(g_stat.PARA_FLG)
			{
				current_temp1=para_curr_disp;
				current_temp2=para_curr_disp;
			}
			else if(g_stat.SER_FLG)
			{
				current_temp1=ch1_curr_disp;
				current_temp1=ch1_curr_disp;
			}
			if(current_temp1>CHAN1.curr_ocp_set||current_temp2>CHAN2.curr_ocp_set)
			{
			 OutPutOff();
			if(current_temp1>CHAN1.curr_ocp_set)
				{
				draw_ocp_flg(1,0);
				}
			if(current_temp2>CHAN2.curr_ocp_set)
				{
				draw_ocp_flg(1,1);
				}
               	
			}
		}
	}
	if(set_reflesh&&cur_page==page_main&&main_info_flg==0)
	{
	
	}
	
	if(disp_reflesh&&cur_page==page_main&&main_info_flg==0)
	{
		if(CH1_STAT||CH2_STAT)
		{	

		}
	}

}
void PAGE_CAL_MAIN(void)
{
	CPU_SR         cpu_sr;
	u16 temp;
	u8 i;
	if(g_stat.cal_set_reflesh)
	{
		g_stat.cal_set_reflesh=0;
	    SetReflsehAll();
	}	
	if(set_reflesh&&cur_page==page_main&&main_info_flg==0)
	{
	
	}
	
	if(disp_reflesh&&cur_page==page_main&&main_info_flg==0)
	{
		if(CH1_STAT||CH2_STAT)
		{	

		}
	}

}
void deal_main_info(u8 INFO_NO )
{

}
extern u8 Adc_get_clear;
extern u8 fine_set_flg;
void Main_Disp_reflesh(void)
{
	//CPU_SR         cpu_sr;
	u8 tm_set;
    double ad_value;
	if(CH1_STAT&&CH2_STAT)
	{
		tm_set=1;
	}
	else
	{
		tm_set=2;
	}
	if(CH1_STAT&&Adc_get_clear==0)
	{

		{
		if(ch1_volt_reflesh)
		{
		if(fine_set_flg==0)
		{
            ch1_volt_reflesh=0;
		}

		{

		}
		}
		if(ch1_volt_reflesh==0&&fine_set_flg==0)
		{
		    Volt_Cur_Change(&CHAN1,0);
		}
		if(main_cur_location==main_location_curr)
		{
            Volt_Cur_Change(&CHAN1,0);
		}
		if(ch1_curr_reflesh)
		{
		if(fine_set_flg==0)
		{
            ch1_curr_reflesh=0;
		}

		}
		if(ch1_curr_reflesh==0&&fine_set_flg==0)
		{
		    Curr_Cur_Change(&CHAN1,0);
		}
		if(main_cur_location==main_location_volt)
		{
            Curr_Cur_Change(&CHAN1,0);
		}
		//if(ch1_volt_reflesh==0&&ch1_curr_reflesh==0)
		{

			ch1_pow_disp=ch1_curr_disp*ch1_volt_disp;  
           // ch1_pow_disp=0.5;
			DISP_POWER(ch1_pow_disp,0);
		}
		}
	}
	if(CH2_STAT&&Adc_get_clear==0)
	{

		{	  	 
			if(ch2_volt_reflesh)
			{
				if(fine_set_flg==0)
        		{
                    ch2_volt_reflesh=0;
        		}

			}
			if(ch2_volt_reflesh==0&&fine_set_flg==0)
			{
				Volt_Cur_Change(&CHAN2,0);
			}
		if(main_cur_location==main_location_curr)
		{
            Volt_Cur_Change(&CHAN2,0);
		}
			if(ch2_curr_reflesh)
			{
    		    if(fine_set_flg==0)
		        {
                    ch2_curr_reflesh=0;
		        }

			}
			if(ch2_curr_reflesh==0&&fine_set_flg==0)
			{
				Curr_Cur_Change(&CHAN2,0); 
			}
			if(main_cur_location==main_location_volt)
    		{
                Curr_Cur_Change(&CHAN2,0);
    		}
			//if(ch2_volt_reflesh==0&&ch2_curr_reflesh==0)
			{

			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				if(g_stat.SER_FLG)
					ch2_pow_disp=ch1_curr_disp*ch2_volt_disp;  
				else
					ch2_pow_disp=ch1_curr_disp*ch1_volt_disp;  
			}
			else
				ch2_pow_disp=ch2_curr_disp*ch2_volt_disp; 
				//ch2_pow_disp=0.5;			
				DISP_POWER(ch2_pow_disp,1);
				ad_value=ch3_volt_disp;
				if(fine_set_flg&&CHAN_SEL==CH3_SEL)
				{
					ad_value=CHAN3.volt_set;
				}
				disp_ch3_volt_Read(ad_value);
			}
		}
	} 
}
void disp_ch3_volt_Read(double value)
{
	HT1621_Disp_CH3_volt(value);

}
void disp_ch3_set_volt(double value)
{
	HT1621_Disp_CH3_volt(value);

}

void DISP_POWER(float val,u16 offset)
{
	//val=0.5;
    HT1621_Disp_power(offset,val);
}
void DISP_CURRENT_CAL_DISP(float val,u16 offset)
{
	//val=0.5;
    HT1621_Disp_Cal_curret(offset,val);
}

static void _DrawIt1(void * pData) 
{

}

void Channel_Change(void)
{
	u8 i,j;
	OS_CPU_SR  cpu_sr ;


}

void Enter_Mainpage(void)
{
	g_stat.store_page_flg=0;
	g_stat.time_page_flg=0;
	g_stat.version_page_flg=0;

}

void Channel_Sel(u8 channel)
{
	 

}
void windowsdisp(u8 channel,P_SET_PARA pt_PARA)
{
	u16 offset;
	float temp;


	
}
void SET_TIMER_REFLESH(u8 chanel,P_SET_PARA pt_PARA)
{
	u16 temp,offset;
	if(chanel==CH1_SEL)
	{
		CH1_SET_FLG=1;
		offset=0;
	}
	else
	{
		CH2_SET_FLG=1;
		offset=238;
	}

	pt_PARA->Pow_set=pt_PARA->curr_set*pt_PARA->volt_set;  
	if(CH1_STAT&&chanel==CH1_SEL)
	{
		goto set_end;
	}
	else if(CH2_STAT&&chanel==CH2_SEL)
	{
		goto set_end;
	}
	if(main_cur_location==main_location_volt&&CHAN_STAT==CH1_SEL&&chanel==CH1_SEL)
	{

	}
	else if(main_cur_location==main_location_volt&&CHAN_STAT==CH2_SEL&&chanel==CH2_SEL)
	{

	}
	else
	{

	}
	Volt_Disp(pt_PARA->volt_set,offset,pt_PARA->DISP_MODE);
	if(main_cur_location==main_location_curr&&CHAN_STAT==CH1_SEL&&chanel==CH1_SEL)
	{

	}
	else if(main_cur_location==main_location_curr&&CHAN_STAT==CH2_SEL&&chanel==CH2_SEL)
	{

	}
	else
	{

	}
	Curr_Disp(pt_PARA->curr_set,offset,pt_PARA->DISP_MODE);
	if(pt_PARA->DISP_MODE==0)
	{
		DISP_POWER(pt_PARA->Pow_set,offset);   
	}
	set_end:	
	
}
extern u8 ch1OVPLedBuf,ch2OVPLedBuf,beepLedbuf;
void Mode_Disp(u8 channel)
{
	

	if(channel==1)
	{

		

		
		if(CH1_STAT)
		{
		if(CH1Mode_flg==0)
		{

		    SET_BIT(ch1OVPLedBuf, LED_BIT2); 
		    CLEAR_BIT(ch1OVPLedBuf, LED_BIT3); 
			WriteAll_1621_CH1(8,&ch1OVPLedBuf,1);

		}
		else
		{
		    SET_BIT(ch1OVPLedBuf, LED_BIT3); 
		    CLEAR_BIT(ch1OVPLedBuf, LED_BIT2); 
			WriteAll_1621_CH1(8,&ch1OVPLedBuf,1);
		}
			if(CH1Mode_flg==0)
			{


				LED_CH1_CV_ON ;
			}
			else
			{

				LED_CH1_CC_ON;
			}

		}
	}
	else if(channel==2)
	{

		

		
		if(CH2_STAT)
		{
			if(g_stat.PARA_FLG)
				{
				SET_BIT(ch2OVPLedBuf, LED_BIT3); 
				CLEAR_BIT(ch2OVPLedBuf, LED_BIT2); 
				WriteAll_1621_CH1(30,&ch2OVPLedBuf,1);
				
				LED_CH2_CC_ON;

				}
        else
        	{
		if(CH2Mode_flg==0)
		{

		    SET_BIT(ch2OVPLedBuf, LED_BIT2); 
		    CLEAR_BIT(ch2OVPLedBuf, LED_BIT3); 
			WriteAll_1621_CH1(30,&ch2OVPLedBuf,1);
			CH2CV_mode_enable=1;
			LED_CH2_CV_ON ;

		}
		else
		{
		    SET_BIT(ch2OVPLedBuf, LED_BIT3); 
		    CLEAR_BIT(ch2OVPLedBuf, LED_BIT2); 
			WriteAll_1621_CH1(30,&ch2OVPLedBuf,1);

			LED_CH2_CC_ON;
	
		}
        	}
		
		}
	}
	
}

void DealSerial(void)
{

}

