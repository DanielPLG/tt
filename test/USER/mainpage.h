#ifndef   _MAINPAGE_H_
#define   _MAINPAGE_H_

#ifdef 	MAINPAGE_GLOBALS
#define   MAIN_EXT
#else
#define   MAIN_EXT  extern
#endif
u8 CH1_STAT;
u8 CH2_STAT;
u8  para_ch1_on_flg=0;
cal_data Cal_Volt,Cal_VoltSet,Cal_Curr,Cal_CurrSet;
Cal_K    K_Volt,K_Curr;
Cal_K  PARA_K[12];
extern u16 framebufer_shift;

float ch1_volt_disp,ch2_volt_disp,ch3_volt_disp ;
float ch1_curr_disp,ch2_curr_disp,para_curr_disp ;
float ch1_volt_disp_cal,ch2_volt_disp_cal,ch3_volt_disp_cal ;
float ch1_curr_disp_cal,ch2_curr_disp_cal ;
float ch1_pow_disp,ch2_pow_disp ;


float volt_step,curr_step;
P_SET_PARA	 pt_PARA;
SET_PARA CHAN1,CHAN2,CHAN3;
DISP_PARA DISP_CH1,DISP_CH2;
P_DISP_PARA	 pt_DISP;
TIMER_PARA TIMER_CH1,TIMER_CH2;
P_TIMER_PARA	 pt_TIMER;
STORE_PARA  para_store;
P_STORE_PARA	 p_para_store;
MAIN_EXT short ch1_volt_reflesh,ch2_volt_reflesh,ch1_volt_cnt,ch2_volt_cnt;
MAIN_EXT u32 ch1_curr_reflesh,ch2_curr_reflesh,ch1_curr_cnt,ch2_curr_cnt;
void Main_Disp_reflesh(void);
void PAGE_MAIN(void);
void Channel_Sel(u8 channel);
void PAGE_TEST(void);
void MAIN_TIMER_SET(u8 flg,P_SET_PARA pt_PARA);

#endif

