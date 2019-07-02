#include <includes.h>
#include "USBTMC.h"
#include "USB488.h"
#include "usbreq.h"
#include "comm.h"
#include "scpi.h"
//#include "Bitmap.h"
#include <stdio.h>
#include <string.h>
unsigned char scpi_Interface_type=0;
void MEAsure_OVP(void);
void MEAsure_OCP(void);


void  CH3_read_STATus(void);
void OUTPut_OVP(void);
void OUTPut_OCP(void);
void  OVER_CURRENT(void);

void  OVER_VOLTage(void);

void floattostr2(char*buf,double tmp);

extern u8 SCPI_CH1_SET,SCPI_CH2_SET,SCPI_CH3_SET,SCPI_OVP,SCPI_OCP;
u8 USB_SEND_FLOAT(float tmp);
void Qry_CH3_VOLTage(void);
void CH1_MEAsure_VOLTage(void);

void CH3_VOLTage(void);

void CURRent(void);
void VOLTage(void);
void Qry_CURRent(void);
void CH3_MEAsure_VOLTage(void);
	

void Qry_VOLTage(void);
void Serial_Query( void );
void ReadAll(void);
void SCPI_BMP_GET(void);
void floattostr(char*buf,float tmp);
char *s_SofewareVersion="1.01.01.01.05";
char *s_HardwareVersion="V1.1";
extern char  * apParamStr[MAX_CMD_PARAMS];   
u8 LcdBuffer[480*3];
u16 REG_STAT;
u8	SCPI_CH1_FLG,SCPI_CH2_FLG;
u8	SCPI_CH1ON_SET,SCPI_CH2ON_SET;
u8 Scpi_Measure_Qury;
char    s1_MEASure_CURRent[]  = "MEASure:CURRent?";
char    s2_MEASure_CURRent[]  = "MEAS:CURRent?";
char    s3_MEASure_CURRent[]  = "MEAS:CURR?";
char    s4_MEASure_CURRent[]  = "MEASure:CURR?";
char    s1_MEASure_VOLTage[]  = "MEASure:VOLTage?";	
char    s2_MEASure_VOLTage[]  = "MEAS:VOLTage?";
char    s3_MEASure_VOLTage[]  = "MEAS:VOLT?";
char    s4_MEASure_VOLTage[]  = "MEASure:VOLT?";

char    s1_CH1_MEASure_CURRent[]  = "CH1:MEASure:CURRent?";
char    s2_CH1_MEASure_CURRent[]  = "CH1:MEAS:CURRent?";
char    s3_CH1_MEASure_CURRent[]  = "CH1:MEAS:CURR?";
char    s4_CH1_MEASure_CURRent[]  = "CH1:MEASure:CURR?";
char    s1_CH1_MEASure_VOLTage[]  = "CH1:MEASure:VOLTage?";	
char    s2_CH1_MEASure_VOLTage[]  = "CH1:MEAS:VOLTage?";
char    s3_CH1_MEASure_VOLTage[]  = "CH1:MEAS:VOLT?";
char    s4_CH1_MEASure_VOLTage[]  = "CH1:MEASure:VOLT?";

char    s1_CH2_MEASure_CURRent[]  = "CH2:MEASure:CURRent?";
char    s2_CH2_MEASure_CURRent[]  = "CH2:MEAS:CURRent?";
char    s3_CH2_MEASure_CURRent[]  = "CH2:MEAS:CURR?";
char    s4_CH2_MEASure_CURRent[]  = "CH2:MEASure:CURR?";
char    s1_CH2_MEASure_VOLTage[]  = "CH2:MEASure:VOLTage?";	
char    s2_CH2_MEASure_VOLTage[]  = "CH2:MEAS:VOLTage?";
char    s3_CH2_MEASure_VOLTage[]  = "CH2:MEAS:VOLT?";
char    s4_CH2_MEASure_VOLTage[]  = "CH2:MEASure:VOLT?";

char    s1_CH3_MEASure_VOLTage[]  = "CH2:MEASure:VOLTage?";	
char    s2_CH3_MEASure_VOLTage[]  = "CH2:MEAS:VOLTage?";
char    s3_CH3_MEASure_VOLTage[]  = "CH2:MEAS:VOLT?";
char    s4_CH3_MEASure_VOLTage[]  = "CH2:MEASure:VOLT?";




char    s1_MEASure_POWEr[]  = "MEASure:POWEr?";	
char    s2_MEASure_POWEr[]  = "MEAS:POWEr?";
char    s3_MEASure_POWEr[]  = "MEAS:POWE?";	
char    s4_MEASure_POWEr[]  = "MEASure:POWE?";
char    s_idn[]  = "*IDN?";
char    s_sav[]  = "*SAV";
char    s_rcl[]  = "*RCL";
char    s_cal_param_cls[]  = "*CALCLS";
char    s_cal_para_st[]  = "*CALST";
char    s1_INSTrument[]  = "INSTrument";
char    s5_INSTrument[]  = "INST";

char    s_tst[]  = "*CALRCL";
char    sa_brument[]  = "INTument";
char    s2_INSTrument[]  = "INSTrument?";
char    s3_INSTrument[]  = "INST";
char    s4_INSTrument[]  = "INST?";
char    s1_QCURRent[]  = "CURRent?";	
char    s2_QCURRent[]  = "CURR?";
#ifdef CH1_RIGHT
char    s1_CH1_QCURRent[]  = "CH2:CURRent?";	
char    s2_CH1_QCURRent[]  = "CH2:CURR?";	
char    s1_CH2_QCURRent[]  = "CH1:CURRent?";	
char    s2_CH2_QCURRent[]  = "CH1:CURR?";
char    s1_CH1_QVOLTage[]  = "CH2:VOLTage?";	
char    s2_CH1_QVOLTage[]  = "CH2:VOLT?";	
char    s1_CH2_QVOLTage[]  = "CH1:VOLTage?";	
char    s2_CH2_QVOLTage[]  = "CH1:VOLT?";
#else
char    s1_CH1_QCURRent[]  = "CH1:CURRent?";	
char    s2_CH1_QCURRent[]  = "CH1:CURR?";	
char    s1_CH2_QCURRent[]  = "CH2:CURRent?";	
char    s2_CH2_QCURRent[]  = "CH2:CURR?";
char    s1_CH1_QVOLTage[]  = "CH1:VOLTage?";	
char    s2_CH1_QVOLTage[]  = "CH1:VOLT?";	
char    s1_CH2_QVOLTage[]  = "CH2:VOLTage?";	
char    s2_CH2_QVOLTage[]  = "CH2:VOLT?";

char    s1_CH3_QVOLTage[]  = "CH3:VOLTage?";	
char    s2_CH3_QVOLTage[]  = "CH3:VOLT?";

#endif
char    s1_QVOLTage[]  = "VOLTage?";	
char    s2_QVOLTage[]  = "VOLT?";
#ifdef CH1_RIGHT
char    s1_CH1_CURRent[]  = "CH2:CURRent";	
char    s2_CH1_CURRent[]  = "CH2:CURR";	
char    s1_CH2_CURRent[]  = "CH1:CURRent";	
char    s2_CH2_CURRent[]  = "CH1:CURR";
#else
char    s1_CH1_CURRent[]  = "CH1:CURRent";	
char    s2_CH1_CURRent[]  = "CH1:CURR";	
char    s1_CH2_CURRent[]  = "CH2:CURRent";	
char    s2_CH2_CURRent[]  = "CH2:CURR";
#endif
char    s1_CURRent[]  = "CURRent";	
char    s2_CURRent[]  = "CURR";
#ifdef CH1_RIGHT
char    s1_CH1_VOLTage[]  = "CH2:VOLTage";	
char    s2_CH1_VOLTage[]  = "CH2:VOLT";	
char    s1_CH2_VOLTage[]  = "CH1:VOLTage";	
char    s2_CH2_VOLTage[]  = "CH1:VOLT";
#else
char    s1_CH1_VOLTage[]  = "CH1:VOLTage";	
char    s2_CH1_VOLTage[]  = "CH1:VOLT";	
char    s1_CH2_VOLTage[]  = "CH2:VOLTage";	
char    s2_CH2_VOLTage[]  = "CH2:VOLT";

char    s1_CH3_VOLTage[]  = "CH3:VOLTage";	
char    s2_CH3_VOLTage[]  = "CH3:VOLT";

#endif
char    s1_OVP_STATUS[]  = "OVP:STAT";
char    s2_OVP_STATUS[]  = "OVP:STATus";

char    s1_OCP_STATUS[]  = "OCP:STAT";
char    s2_OCP_STATUS[]  = "OCP:STATus";

char    s1_OVP_SETTING[]  = "OVP:SETting";
char    s2_OVP_SETTING[]  = "OVP:SET";

char    s1_OCP_SETTING[]  = "OCP:SETting";
char    s2_OCP_SETTING[]  = "OCP:SET";

char    s1_OVP_VALUE[]  = "OVP:VALUE";
char    s1_OCP_VALUE[]  = "OVP:VALUE?";





char    s1_VOLTage[]  = "VOLTage";	
char    s2_VOLTage[]  = "VOLT";
char    s1_OUTPut[]  = "OUTPut";	
char    s2_OUTPut[]  = "OUTP";
char    s1_OUTPut_TRACK[]  = "OUTPut:TRACk";	
char    s2_OUTPut_TRACK[]  = "OUTP:TRACk";
char    s1_OUTPut_WAVE[]  = "OUTPut:WAVE";	
char    s2_OUTPut_WAVE[]  = "OUTP:WAVE";
char    s1_QOUTPut_WAVE[]  = "OUTPut:WAVE?";	
char    s2_QOUTPut_WAVE[]  = "OUTP:WAVE?";
char    s1_TIMEr[]  = "TIMEr";	
char    s2_TIMEr[]  = "TIME";
char    s1_TIMEr_SET[]  = "TIMEr:SET";	
char    s2_TIMEr_SET[]  = "TIME:SET";
char    s1_QTIMEr_SET[]  = "TIMEr:SET?";	
char    s2_QTIMEr_SET[]  = "TIME:SET?";	
char    s1_SYSTem_ERRor[]  = "SYSTem:ERRor?";	
char    s2_SYSTem_ERRor[]  = "SYST:ERRor?";
char    s3_SYSTem_ERRor[]  = "SYST:ERR?";
char    s4_SYSTem_ERRor[]  = "SYSTem:ERR?";
char    s1_SYSTem_VERSion[]  = "SYSTem:VERSion?";	
char    s2_SYSTem_VERSion[]  = "SYST:VERSion?";
char    s3_SYSTem_VERSion[]  = "SYST:VERS?";
char    s4_SYSTem_VERSion[]  = "SYSTem:VERS?";
char    s1_SYSTem_STATus[]  = "SYSTem:STATus?";	
char    s2_SYSTem_STATus[]  = "SYST:STATus?";
char    s3_SYSTem_STATus[]  = "SYST:STAT?";
char    s4_SYSTem_STATus[]  = "SYSTem:STAT?";	

char    s1_CH3_READ_STATus[]  = "CH3:STATus?";	

char    s2_CH3_READ_STATus[]  = "CH3:STAT?";



char    s1_CALibration_VOLTage[]  = "CALibration:VOLTage";
char    s2_CALibration_VOLTage[]  = "CAL:VOLTage";
char    s3_CALibration_VOLTage[]  = "CAL:VOLT";	
char    s4_CALibration_VOLTage[]  = "CALibration:CURR";	
char    s1_CALibration_CURRent[]  = "CALibration:CURRent";
char    s2_CALibration_CURRent[]  = "CAL:CURRent";
char    s3_CALibration_CURRent[]  = "CAL:CURR";	
char    s4_CALibration_CURRent[]  = "CALibration:VOLT";				   
// Device-specific command definitions (mnemonic strings) ...

char    s_count_read[]    = "BMP:READ?";
char    s_read_product[]   = "IDN-SGLT-PRI?";
char    s_key[]     = "**READID";
char    s_upgrade[]   = "*UPGRADE";
char    s_serial_set[]     = "SRLN";
char    s_serial_enq[]     = "SRLN?";
char    s_read_all[]        = "*READALL?";
char    s_debug_flags[]   = "DEBUG:FLAGS?";

// Command table

const commandTable  Command[] =
{
	{ s1_MEASure_CURRent,1,  MEASure_CURRent ,   TRUE  },
	{ s2_MEASure_CURRent, 1, MEASure_CURRent ,   TRUE  },
	{ s3_MEASure_CURRent,1,  MEASure_CURRent ,   TRUE  },
	{ s4_MEASure_CURRent,1,  MEASure_CURRent ,   TRUE  },
	{ s1_MEASure_VOLTage,1,  MEAsure_VOLTage ,   TRUE  },
	{ s2_MEASure_VOLTage,1,  MEAsure_VOLTage ,   TRUE  },
	{ s3_MEASure_VOLTage,1,  MEAsure_VOLTage ,   TRUE  },
	{ s4_MEASure_VOLTage,1,  MEAsure_VOLTage ,   TRUE  },

	{ s1_CH3_MEASure_VOLTage,1,  CH3_MEAsure_VOLTage ,   TRUE  },
	{ s2_CH3_MEASure_VOLTage,1,  CH3_MEAsure_VOLTage ,   TRUE  },
	{ s3_CH3_MEASure_VOLTage,1,  CH3_MEAsure_VOLTage ,   TRUE  },
	{ s4_CH3_MEASure_VOLTage,1,  CH3_MEAsure_VOLTage ,   TRUE  },

	
	{ s1_MEASure_POWEr,1,MEASure_POWEr,   TRUE  },
	{ s2_MEASure_POWEr,1,MEASure_POWEr,   TRUE  },
	{ s3_MEASure_POWEr,1,MEASure_POWEr,   TRUE  },
	{ s4_MEASure_POWEr,1,MEASure_POWEr,   TRUE  },
	{ s_idn,           0,   SCPI_IDN,           TRUE  },
	{ s_sav,           1,   SAV,                FALSE },
	{ s_rcl,           1,   RCL,               FALSE },
	{ s_cal_param_cls,         1,   CalParam_Cls,           FALSE  },
	{ s_cal_para_st,           0,   CalParam_Store,           FALSE   },
	{ s1_INSTrument,           1,   INSTrument,           FALSE },
	{ s_tst,           0,   qry_tst,           FALSE  },
	{ s_read_product,   0,   cmd_count_reset,   TRUE },
	{ s_upgrade,   0,   Upgrade_Rst,     FALSE   },	
	{ s_key,     0,   cmd_KEY,     TRUE },
	{ s_serial_set,     1,   Serial_Set,     FALSE },
	{ s3_INSTrument,   1,   INSTrument,     FALSE },
	{ s_read_all,        0,   ReadAll,          TRUE  },
	{ s_debug_flags,   0,   qry_debug_flags,   TRUE  },
	{ s_serial_enq,     0,  Serial_Query,   TRUE },
	{ s2_INSTrument,   0,  qry_INSTrument ,   TRUE  },
	{ s4_INSTrument,   0,  qry_INSTrument ,   TRUE  },

	{ s1_CH1_QCURRent, 0,Qry_CH1_CURRent,  TRUE },
	{ s2_CH1_QCURRent, 0,Qry_CH1_CURRent,  TRUE },
	{ s1_CH2_QCURRent, 0,Qry_CH2_CURRent,  TRUE },
	{ s2_CH2_QCURRent, 0,Qry_CH2_CURRent,  TRUE },	
	{ s1_CH1_CURRent, 1,CH1_CURRent,  FALSE },
	{ s2_CH1_CURRent, 1,CH1_CURRent,  FALSE },
	{ s1_CH2_CURRent, 1,CH2_CURRent,  FALSE },
	{ s2_CH2_CURRent, 1,CH2_CURRent,  FALSE },
	{ s1_QCURRent, 0,Qry_CURRent,  TRUE },
	{ s2_QCURRent, 0,Qry_CURRent,  TRUE },
	{ s1_CURRent, 1,CURRent,  FALSE },
	{ s2_CURRent, 1,CURRent,  FALSE },
	{s1_QVOLTage , 0,  Qry_VOLTage ,   TRUE  },
	{s2_QVOLTage , 0,  Qry_VOLTage ,   TRUE  },	
	{s1_CH1_QVOLTage , 0,  Qry_CH1_VOLTage ,   TRUE  },
	{s2_CH1_QVOLTage , 0,  Qry_CH1_VOLTage ,   TRUE  },	
	{s1_CH2_QVOLTage , 0,  Qry_CH2_VOLTage ,   TRUE  },
	{s2_CH2_QVOLTage , 0,  Qry_CH2_VOLTage ,   TRUE  },
	{s1_VOLTage , 1,  VOLTage ,   FALSE },
	{s2_VOLTage , 1,  VOLTage ,   FALSE },	
	{s1_CH1_VOLTage , 1,  CH1_VOLTage ,   FALSE },
	{s2_CH1_VOLTage , 1,  CH1_VOLTage ,   FALSE },	
	{s1_CH2_VOLTage , 1,  CH2_VOLTage ,   FALSE  },
	{s2_CH2_VOLTage , 1,  CH2_VOLTage ,   FALSE  },

	{s1_OVP_STATUS , 1,  OUTPut_OVP ,   FALSE  },
	{s1_OVP_STATUS , 1,  OUTPut_OVP ,   FALSE  },
	{s1_OCP_STATUS , 1,  OUTPut_OCP ,	FALSE  },
	{s2_OCP_STATUS , 1,  OUTPut_OCP ,	FALSE  },

	{s1_OVP_SETTING , 2,  OVER_VOLTage ,   FALSE  },
	{s2_OVP_SETTING , 2,  OVER_VOLTage ,   FALSE  },
	{s1_OCP_SETTING , 2,  OVER_CURRENT ,	FALSE  },
	{s2_OCP_SETTING , 2,  OVER_CURRENT ,	FALSE  },
	{ s1_OVP_VALUE,1,  MEAsure_OVP ,   TRUE  },
	{ s1_OCP_VALUE,1,  MEAsure_OCP ,   TRUE  },


	{s1_CH3_VOLTage , 1,  CH3_VOLTage ,   FALSE  },
	{s2_CH3_VOLTage , 1,  CH3_VOLTage ,   FALSE  },

	{s1_CH3_QVOLTage , 0,  Qry_CH3_VOLTage ,   TRUE  },
	{s2_CH3_QVOLTage , 0,  Qry_CH3_VOLTage ,   TRUE  },


	{s1_OUTPut ,1,OUTPut_STATe, FALSE  },
	{s2_OUTPut ,1,OUTPut_STATe, FALSE  },

	{ s1_OUTPut_TRACK,1,OUTPut_TRACk,FALSE },
	{ s2_OUTPut_TRACK,1,OUTPut_TRACk,FALSE },





	{ s1_SYSTem_ERRor,  0,SYSTem_ERRor ,TRUE  },
	{ s2_SYSTem_ERRor,  0,SYSTem_ERRor ,TRUE  },
	{ s3_SYSTem_ERRor,  0,SYSTem_ERRor ,TRUE  },
	{ s4_SYSTem_ERRor,  0,SYSTem_ERRor ,TRUE  },

	{ s1_SYSTem_VERSion,0,SYSTem_VERSion,TRUE  },
	{ s2_SYSTem_VERSion,0,SYSTem_VERSion,TRUE  },
	{ s3_SYSTem_VERSion,0,SYSTem_VERSion,TRUE  },
	{ s4_SYSTem_VERSion,0,SYSTem_VERSion,TRUE  },

	{ s1_CH3_READ_STATus,0,CH3_read_STATus ,TRUE},
	{ s1_CH3_READ_STATus,0,CH3_read_STATus ,TRUE},


	{ s1_SYSTem_STATus,0,SYSTem_STATus ,TRUE},
	{ s2_SYSTem_STATus,0,SYSTem_STATus ,TRUE},
	{ s3_SYSTem_STATus,0,SYSTem_STATus ,TRUE},
	{ s4_SYSTem_STATus,0,SYSTem_STATus ,TRUE},
	{ s1_CALibration_VOLTage,3,CALibration_VOLTage,FALSE },
	{ s2_CALibration_VOLTage,3,CALibration_VOLTage,FALSE },
	{ s3_CALibration_VOLTage,3,CALibration_VOLTage,FALSE },
	{ s4_CALibration_VOLTage,3,CALibration_VOLTage,FALSE },
	{ s1_CALibration_CURRent,3,CALibration_CURRent,FALSE},
	{ s2_CALibration_CURRent,3,CALibration_CURRent,FALSE},
	{ s3_CALibration_CURRent,3,CALibration_CURRent,FALSE},
	{ s4_CALibration_CURRent,3,CALibration_CURRent,FALSE},
	
	{ NULL,              0,   NULL,              FALSE }      //  Last entry in cmd table
};
 const SCPI_KEYWORD keywords[]=
{
 {"SYST","SYSTem"},
 {"STAT","STATus"},
 {"CALI","CALibration"},
 {"OUTP","OUTPut"},
 {"MEAS","MEASure"},
 {"CHAN1","CHANnel1"},
 {"CHAN2","CHANnel2"},
 {"CHAN3","CHANnel3"},
 
 {"SAV","SAVe"},
 {"SEC","SECure"},
 {"POW","POWer"},
 {"ERR","ERRor"},
 {"VOLT","VOLTage"},
 {"CURR","CURR"},
 {"VERS","VERSion"},
 {"QUES","QUEStionable"},
 {"OPER","OPERation"},
 {"PROT","PROTection"},
 {"INIT","INITital"},
 
 {"LEV","LEVel"},
 {"ENAB","ENABle"},
 {"COND","CONDition"}, 
 {"STAT","STATe"},
 {"EVEN","EVENt"},
 {"NEXT","NEXT"},
 {"DATA","DATA"},

}
;
const u8 com_cmd[]=
{
  "CLS"
  "ESE"
  "ESR"
  "IDN"
  "SRE"
  "STB"	
};
const SCPI_ERR_INFO ERR[]=
{
 {0,"No Error"}, 
 {1,"Too many numeric suffices in Command Spec"},
 {2,"Run Error"}, 
 {3,"Command keywords were not recognized"}, 
 {4,"Too many command"},
 {5,"Command not allowed"},
 {6,"Wrong type of parameter(s)"}, 
 {7,"Wrong number of parameters"},
 {8,"No Input Command to parse"},
 {9,"Data out of range"},
 {10,"Unmatched bracket"}, 
};
extern char    zIDN_string[];

u8 check_para(char*buf,u8 len)
{
	u8 i,err;
	u8 dot_num=0;
	char Result[10];
	memcpy( Result, buf, strlen(buf) );
	for(i=0;i<len;i++)
	{
		if(Result[i]>0x39||((Result[i]<0x30)&&(Result[i]!=0x2e)))
		{
			err=0 ;
			return err ;
		}
		if(Result[i]==0x2e)
			dot_num++;
	}
	if(dot_num>1)
		return 0;
	
	return 1;
}


u8 check_int_para(char*buf,u8 len)
{
	u8 i,err;
	char Result[10];
	memcpy( Result, buf, strlen(buf) );
	for(i=0;i<len;i++)
	{
		if(Result[i]>0x39||Result[i]<0x30)
		{
			err=0 ;
			return err ;
		}
	}
	return 1;
}
//extern LCDBITMAPFILE lcdBitMapFile;
void SCPI_BMP_GET(void)
{

}

void SCPI_IDN(void)
{
	u8   cBuf[8];
	ERR_Flg=0;
	usbtmc_response_cat( zIDN_string, strlen(zIDN_string) );
	usbtmc_response_cat( para_factory.SERIAL_NO, strlen(para_factory.SERIAL_NO) );

	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	usbtmc_response_cat( s_SofewareVersion, strlen(s_SofewareVersion) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	usbtmc_response_cat( s_HardwareVersion, strlen(s_HardwareVersion) );
	/*	longToDecimalStr( 5, cBuf, 1 );
	usbtmc_response_cat( cBuf, strlen(cBuf) );
	cBuf[0] = '.';
	usbtmc_response_cat( cBuf, 1 );
	longToDecimalStr( BUILD_VER_DEBUG, cBuf, 1 );
	usbtmc_response_cat( cBuf, strlen(cBuf) ); 	 */
}
//extern char  * apParamStr[MAX_CMD_PARAMS]; 

void SAV(void)
{
	u8 i;
	u8 temp;
	ERR_Flg=0;
	if( check_int_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atoi( apParamStr[1] )	;
	if(temp<1||temp>5)
	{
		ERR_Num=9;
		ERR_Flg=1;
		beep_on=1;
	}
	else
	{
		temp=temp-1;
		//set_store(para_store);
		//if(PARA_Saved(PARA_ADDRESS_(temp),&para_store, sizeof(para_store)))
		if(set_store(para_store,PARA_ADDRESS_(temp)))
			ERR_Flg=0;
		else
		{
			ERR_Num=2;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}


u8 file_num,recall_flg;
void RCL(void)
{
	ERR_Flg=0;
	if( check_int_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	file_num=atoi( apParamStr[1] )	;
	if(file_num<1||file_num>5)
	{
		ERR_Num=9;
		ERR_Flg=1;
		beep_on=1;
	}
	else
	{
		file_num=file_num-1;
		recall_flg=1;
		ERR_Flg=0;
	}
}


void qry_INSTrument(void)
	{
	if(CHAN_SEL==CH2_SEL) 
		usbtmc_response_cat( CH2, strlen(CH2) );
	else if(CHAN_SEL==CH1_SEL) 
		usbtmc_response_cat( CH1, strlen(CH1) );
	else
		usbtmc_response_cat( CH3, strlen(CH3) );
	
	ERR_Flg=0;
	}


void MEASure_CURRent(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_SEL==CH1_SEL)
		{
			if(CH1_STAT)
			{
				temp=ch1_curr_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(CHAN_SEL==CH2_SEL)
		{
			if(CH2_STAT)
			{
				temp=ch2_curr_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
			{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;

		}
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			if(CH2_STAT)
			{
				temp=ch2_curr_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			if(CH1_STAT)
			{
				temp=ch1_curr_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}
void CH3_MEAsure_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	//if(Scpi_Measure_Qury)
	{

		//if(CHAN_STAT==CH1_SEL)
		{
			if(CH1_STAT)
			{
				temp=ch3_volt_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		
		
	}

	
}

void CH1_MEAsure_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_STAT==CH1_SEL)
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp;
				floattostr(Result,temp);
				// printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				// printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}
void MEAsure_OVP(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_SEL==CH1_SEL)
		{
         temp=CHAN1.volt_ovp_set;
		 floattostr(Result,temp);
		 usbtmc_response_cat( Result, strlen(Result) );
		}
		else if(CHAN_SEL==CH2_SEL)
		{
			temp=CHAN2.volt_ovp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}
		else
			{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;

		}
		
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			temp=CHAN2.volt_ovp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			temp=CHAN1.volt_ovp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}


		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}

void MEAsure_OCP(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_SEL==CH1_SEL)
		{
         temp=CHAN1.curr_ocp_set;
		 floattostr(Result,temp);
		 usbtmc_response_cat( Result, strlen(Result) );
		}
		else if(CHAN_SEL==CH2_SEL)
		{
			temp=CHAN2.curr_ocp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}
		else
			{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;

		}
		
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			temp=CHAN2.curr_ocp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			temp=CHAN1.curr_ocp_set;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );

		}


		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}

void MEAsure_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_SEL==CH1_SEL)
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(CHAN_SEL==CH2_SEL)
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{

		if(CH1_STAT)
		{
			temp=ch3_volt_disp;
			floattostr(Result,temp);
			usbtmc_response_cat( Result, strlen(Result) );
		}
		else
		{
			floattostr(Result,0);
			usbtmc_response_cat( Result, strlen(Result) );
		}

		}
		
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp;
				floattostr(Result,temp);
				// printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				// printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(strnmatch( apParamStr[1], CH3, 3 ))
		{
			if(CH1_STAT)
			{
				temp=ch3_volt_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}

		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}


void MEASure_POWEr(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
	Result[i]=0;
	ERR_Flg=0;
	if(Scpi_Measure_Qury)
	{
		Scpi_Measure_Qury=0;
		if(CHAN_STAT==CH1_SEL)
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp*ch1_curr_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
			//	printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp*ch2_curr_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
			//	printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
	}
	else
	{
		if(strnmatch( apParamStr[1], CH2, 3 )	)
		{
			if(CH2_STAT)
			{
				temp=ch2_volt_disp*ch2_curr_disp;
				floattostr(Result,temp);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else if(strnmatch( apParamStr[1], CH1, 3 ))
		{
			if(CH1_STAT)
			{
				temp=ch1_volt_disp*ch1_curr_disp;
				floattostr(Result,temp);
				usbtmc_response_cat( Result, strlen(Result) );
			}
			else
			{
				floattostr(Result,0);
				//printf(Result);
				usbtmc_response_cat( Result, strlen(Result) );
			}
		}
		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}
}


void CH1_CURRent(void)
{

	float temp=0;

	char Result[10];
	ERR_Flg=0;
	if( check_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atof( apParamStr[1] )	;
#ifdef MASTER_RIGHT	
	if((g_stat.PARA_FLG||g_stat.SER_FLG))
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#else
	if(TIMER_CH1.timer_open)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#endif 

	else
	{
		if(temp>(Curr_max+0.0009)||temp<0)
		{
			ERR_Num=9;
			ERR_Flg=1;
			beep_on=1;
		}
		else
		{
			floattostr(Result,temp);
			temp=atof(Result);
			CHAN1.curr_set=temp;
			SCPI_CH1_SET=1;
#ifndef MASTER_RIGHT				
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				CHAN2.curr_set=CHAN1.curr_set;
				SCPI_CH2_SET=1;
			}
#endif 			
		}
	}

}


void CH2_CURRent(void)
{

	float tmp;
	float temp=0;
	char Result[10];
	ERR_Flg=0;
	if( check_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atof( apParamStr[1] )	;
#ifdef MASTER_RIGHT
	if(TIMER_CH2.timer_open)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#else
	if((g_stat.PARA_FLG||g_stat.SER_FLG))
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#endif 
	else
	{
		if(temp>(Curr_max+0.0009)||temp<0)
		{
			ERR_Num=9;
			ERR_Flg=1;
			beep_on=1;
		}
		else
		{
			floattostr(Result,temp);
			temp=atof(Result);
			CHAN2.curr_set=temp;
			SCPI_CH2_SET=1;
#ifdef MASTER_RIGHT	
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				CHAN1.curr_set=CHAN2.curr_set;
				SCPI_CH1_SET=1;
			}
#endif 
		}
	}
}


void CURRent(void)
{
	if(CHAN_STAT==CH1_SEL)
		CH1_CURRent();
	else
		CH2_CURRent();
}


void Qry_CH1_CURRent(void)
{
	u8 i;
	char Result[10];
	float temp;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	ERR_Flg=0;
	floattostr(Result,CHAN1.curr_set);
	//printf(Result);
	usbtmc_response_cat( Result, strlen(Result) );
}


void Qry_CH2_CURRent(void)
{
	u8 i;
	char Result[10];
	float temp;
	ERR_Flg=0;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	floattostr(Result,CHAN2.curr_set);
	// printf(Result);
	usbtmc_response_cat( Result, strlen(Result) );
}


void  Qry_CURRent(void)
{
	if(CHAN_STAT==CH1_SEL)
		Qry_CH1_CURRent();
	else
		Qry_CH2_CURRent();
}


void CH1_VOLTage(void)
{
	float temp=0;
	char Result[10];
	ERR_Flg=0;
	if( check_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atof( apParamStr[1] )	;
#ifdef MASTER_RIGHT	
	if(TIMER_CH1.timer_open||g_stat.SER_FLG||g_stat.PARA_FLG)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#else
	if(TIMER_CH1.timer_open)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#endif 
	else
	{
		if(temp>Volt_max||temp<0)
		{
			ERR_Num=9;
			ERR_Flg=1;
			beep_on=1;
		}
		else
		{
			floattostr(Result,temp);
			temp=atof(Result);
			CHAN1.volt_set=temp;
			SCPI_CH1_SET=1;
#ifndef MASTER_RIGHT	
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				CHAN2.volt_set=CHAN1.volt_set;
				SCPI_CH2_SET=1;
			}
#endif 
		}
	}
}


void CH2_VOLTage(void)
{
	float temp=0;
	char Result[10];
	ERR_Flg=0;
	if( check_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atof( apParamStr[1] )	;

#ifdef MASTER_RIGHT	
	if(TIMER_CH2.timer_open)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#else
	if(TIMER_CH2.timer_open||g_stat.SER_FLG||g_stat.PARA_FLG)
	{
		ERR_Num=5;
		ERR_Flg=1;
		beep_on=1;
	}
#endif 
	else
	{
		if(temp>Volt_max||temp<0)
		{
			ERR_Num=9;
			ERR_Flg=1;
			beep_on=1;
		}
		else
		{
			floattostr(Result,temp);
			temp=atof(Result);
			CHAN2.volt_set=temp;
			SCPI_CH2_SET=1;
#ifdef MASTER_RIGHT	
			if(g_stat.PARA_FLG||g_stat.SER_FLG)
			{
				CHAN1.volt_set=CHAN2.volt_set;
				SCPI_CH1_SET=1;
			}
#endif 
		}
	}
}
void CH3_VOLTage(void)
{
	float temp=0;
	char Result[10];
	ERR_Flg=0;
	if( check_para(apParamStr[1],strlen(apParamStr[1]))==0	)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	temp=atof( apParamStr[1] )	;


	{
		if(temp>Volt_max_ch3||temp<0)
		{
			ERR_Num=9;
			ERR_Flg=1;
			beep_on=1;
		}
		else
		{
			floattostr(Result,temp);
			temp=atof(Result);
			CHAN3.volt_set=temp;
			SCPI_CH3_SET=1;

		}
	}
}


void VOLTage(void)
{
	if(CHAN_STAT==CH1_SEL)
		CH1_VOLTage();
	else
		CH2_VOLTage();
}


void Qry_CH1_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	ERR_Flg=0;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	floattostr(Result,CHAN1.volt_set);
	// printf(Result);
	usbtmc_response_cat( Result, strlen(Result) );
}


void Qry_CH2_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	ERR_Flg=0;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	floattostr(Result,CHAN2.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
}
void Qry_CH3_VOLTage(void)
{
	u8 i;
	char Result[10];
	float temp;
	ERR_Flg=0;
	for(i=0;i<10;i++)
		Result[i]=0;
	
	floattostr2(Result,CHAN3.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
}

void dmm_set_scpi_Interface_type(unsigned char state)
{
	scpi_Interface_type=state;
	//smp_mgr.type=mode;
}
unsigned char dmm_get_scpi_Interface_type(void)
{
	return scpi_Interface_type;
	
}

void Qry_VOLTage(void)
{
	if(CHAN_SEL==CH1_SEL)
		Qry_CH1_VOLTage();
	else if(CHAN_SEL==CH2_SEL)
		Qry_CH2_VOLTage();
	else
		Qry_CH3_VOLTage();
		
}


void OUTPut_STATe(void)
{
	ERR_Flg=0;

		if(strnmatch( apParamStr[1], "ON", 2 ))
		{
			key_vale=KEY_ALL_ON;
			key_stat=1;
			g_stat.ALL_ON_FLG=0x00;
			//SCPI_CH1_FLG=1;
			//SCPI_CH1ON_SET=1;
		}
		else if(strnmatch( apParamStr[1], "OFF", 3 ))
		{
			key_vale=KEY_ALL_ON;
			key_stat=1;
			g_stat.ALL_ON_FLG=0xff;
			//SCPI_CH1_FLG=0;
			//SCPI_CH1ON_SET=1;
		}
	
	else
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
	}
}
void OUTPut_OVP(void)
	{
		ERR_Flg=0;
	
			if(strnmatch( apParamStr[1], "ON", 2 ))
			{
				if(g_stat.OVP_ON_FLG==0)
				{
				   g_stat.OVP_ON_FLG=(~g_stat.OVP_ON_FLG);
					SCPI_OVP=1;
				}

			}
			else if(strnmatch( apParamStr[1], "OFF", 3 ))
			{
				if(g_stat.OVP_ON_FLG)
				{
				   g_stat.OVP_ON_FLG=0;
				   SCPI_OVP=1;
				}

			}
		
		else
		{
			ERR_Num=6;
			ERR_Flg=1;
			beep_on=1;
		}
	}


void OUTPut_OCP(void)
	{
			ERR_Flg=0;
		
				if(strnmatch( apParamStr[1], "ON", 2 ))
				{
					if(g_stat.OCP_ON_FLG==0)
					{
					   g_stat.OCP_ON_FLG=(~g_stat.OCP_ON_FLG);
						SCPI_OCP=1;
					}
	
				}
				else if(strnmatch( apParamStr[1], "OFF", 3 ))
				{
					if(g_stat.OCP_ON_FLG)
					{
					   g_stat.OCP_ON_FLG=0;
					   SCPI_OCP=1;
					}
	
				}
			
			else
			{
				ERR_Num=6;
				ERR_Flg=1;
				beep_on=1;
			}
		}




void OUTPut_TRACk(void)
{
	u8 temp;
	ERR_Flg=0;
	temp=atoi( apParamStr[1] )	;
	if(temp==0)
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
	else if(temp==1)
	{
		SCPI_SER_FLG=1;
		SCPI_SER_SET=1;
		key_vale=KEY_SER;
		key_stat=1;
	}
	else if(temp==2)
	{
		SCPI_PARA_FLG=1;
		SCPI_PARA_SET=1;
		key_vale=KEY_PARA;
		key_stat=1;
	}
	else 
	{
		ERR_Num=9;
		ERR_Flg=1;
		beep_on=1;
	}
}





u8 scpi_channel,scpi_timer_flg;




void  SYSTem_ERRor(void)
{
	u8 i;
	char Result[4];
	for(i=0;i<4;i++)
		Result[i]=0; 
	
	if(ERR_Flg==0)
		ERR_Num=0;
	
	sprintf(Result,"%d",ERR[ERR_Num].NO); 
	usbtmc_response_cat( Result, strlen(Result) ); 
	Result[0]=' ';
	Result[1]=' ';
	usbtmc_response_cat( Result, 2 ); 
	usbtmc_response_cat( ERR[ERR_Num].ERR_INFO, strlen(ERR[ERR_Num].ERR_INFO) );
	ERR_Flg=0;
}

void  SYSTem_VERSion(void)
{
	u8 i;
	u16 temp;
	u8   cBuf[8];
	u8 Result[10];
	ERR_Flg=0;
	for(i=0;i<10;i++)
		Result[i]=0;
	usbtmc_response_cat( s_SofewareVersion, strlen(s_SofewareVersion) ); 
	//usbtmc_response_cat( TEST, sizeof(TEST) ); 
	/*for(i=0;i<8;i++)
	{
		floattostr(Result, PARA_K[i].a);
		usbtmc_response_cat( Result, strlen(Result) ); 
		cBuf[0] = ',';
		usbtmc_response_cat( cBuf, 1 );
		floattostr(Result, PARA_K[i].b);
		usbtmc_response_cat( Result, strlen(Result) );
		cBuf[0] = ',';
		usbtmc_response_cat( cBuf, 1 ); 
	}	*/   

}


void  Reflesh_Status(void)
{
	REG_STAT=0;
#ifdef CH1_RIGHT	
	if(CH2Mode_flg==0)
		CLEAR_BIT(REG_STAT, (1<<0) );
	else 
		SET_BIT(REG_STAT, (1<<0));
	
	if(CH1Mode_flg==0)
		CLEAR_BIT(REG_STAT, (1<<1) );
	else 
		SET_BIT(REG_STAT, (1<<1));
#else
	if(CH1Mode_flg==0)
		CLEAR_BIT(REG_STAT, (1<<0) );
	else 
		SET_BIT(REG_STAT, (1<<0));
	
	if(CH2Mode_flg==0)
		CLEAR_BIT(REG_STAT, (1<<1) );
	else 
		SET_BIT(REG_STAT, (1<<1));
#endif
	if(g_stat.PARA_FLG)
		SET_BIT(REG_STAT, (2<<2));
	else if(g_stat.SER_FLG)
		SET_BIT(REG_STAT, (3<<2));
	else
		SET_BIT(REG_STAT, (1<<2));
#ifdef CH1_RIGHT	
	if(CH1_STAT)
		SET_BIT(REG_STAT, (1<<4));
	if(g_stat.OVP_ON_FLG)
		SET_BIT(REG_STAT, (1<<5));
	if(g_stat.OCP_ON_FLG)
		SET_BIT(REG_STAT, (1<<6));
	if(TIMER_CH1.timer_open)
		SET_BIT(REG_STAT, (1<<7));
	if(CHAN2.DISP_MODE)
		SET_BIT(REG_STAT, (1<<8));
	if(CHAN1.DISP_MODE)
		SET_BIT(REG_STAT, (1<<9));
#else
	if(CH1_STAT)
		SET_BIT(REG_STAT, (1<<4));
	if(g_stat.OVP_ON_FLG)
		SET_BIT(REG_STAT, (1<<5));
	if(g_stat.OCP_ON_FLG)
		SET_BIT(REG_STAT, (1<<6));
	if(TIMER_CH2.timer_open)
		SET_BIT(REG_STAT, (1<<7));
	if(CHAN1.DISP_MODE)
		SET_BIT(REG_STAT, (1<<8));
	if(CHAN2.DISP_MODE)
		SET_BIT(REG_STAT, (1<<9));
#endif
	
}
extern u8 ch3_ChannelSel;
void  CH3_read_STATus(void)
{
	u8 i;
	char Result[4];
	for(i=0;i<4;i++)
		Result[i]=0; 

	sprintf(Result,"%d",ch3_ChannelSel); 
	usbtmc_response_cat( Result, strlen(Result) ); 


}

void  SYSTem_STATus(void)
{
	char   cBuf[6];
	ERR_Flg=0;
	Reflesh_Status();
	cBuf[0] = '0';
	cBuf[1] = 'x';
	usbtmc_response_cat( cBuf, 2 );
	wordToHexStr( REG_STAT, cBuf );
	usbtmc_response_cat( cBuf, 4 );

}

void  OVER_VOLTage(void)
{
	u8 num;
	u8 err=0;
	float temp;
	ERR_Flg=0;
	if( check_int_para(apParamStr[2],strlen(apParamStr[2]))==0	)
		err=1;

	
	if(err)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	//num=atoi( apParamStr[2] );
	temp=atof( apParamStr[2] )	;

	if(strnmatch( apParamStr[1], CH2, 3 )	)
	{
       CHAN2.volt_ovp_set=temp;
	}
	else if(strnmatch( apParamStr[1], CH1, 3 ))
	{
		CHAN1.volt_ovp_set=temp;
	}
	else if(strnmatch( apParamStr[1], CH3, 3 ))
	{
		CHAN3.volt_ovp_set=temp;
	}

	else
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
	}
}
void  OVER_CURRENT(void)
{
	u8 num;
	u8 err=0;
	float temp;
	ERR_Flg=0;
	if( check_int_para(apParamStr[2],strlen(apParamStr[2]))==0	)
		err=1;

	
	if(err)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	//num=atoi( apParamStr[2] );
	temp=atof( apParamStr[2] )	;

	if(strnmatch( apParamStr[1], CH2, 3 )	)
	{
		CHAN2.curr_ocp_set=temp;
	}
	else if(strnmatch( apParamStr[1], CH1, 3 ))
	{
		CHAN1.curr_ocp_set=temp;
	}
	else if(strnmatch( apParamStr[1], CH3, 3 ))
	{
		CHAN3.curr_ocp_set=temp;
	}

	else
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
	}
}

void  CALibration_VOLTage(void)
{
	u8 num;
	u8 err=0;
	float temp;
	ERR_Flg=0;
	if( check_int_para(apParamStr[2],strlen(apParamStr[2]))==0	)
		err=1;
	if( check_para(apParamStr[3],strlen(apParamStr[3]))==0	)
		err=1;
	
	if(err)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	num=atoi( apParamStr[2] );
	temp=atof( apParamStr[3] )	;
	if(num<1||num>2)
	{
		ERR_Num=9;
		ERR_Flg=1;
		beep_on=1;
	}
	if(strnmatch( apParamStr[1], CH2, 3 )	)
	{
		if(num==1)
		{
			Cal_Volt.Cal_X1=ch2_volt_disp;
			Cal_Volt.Cal_Y1=temp;
			Cal_VoltSet.Cal_X1=temp;
			Cal_VoltSet.Cal_Y1=CHAN2.volt_set;
		}
		else if(num==2)
		{
			Cal_Volt.Cal_X2=ch2_volt_disp;
			Cal_Volt.Cal_Y2=temp;
			Cal_VoltSet.Cal_X2=temp;
			Cal_VoltSet.Cal_Y2=CHAN2.volt_set;
			PARA_K[CH2_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
			PARA_K[CH2_VOLT_SET_CAL]=Cal_Test(Cal_VoltSet);
		}
	}
	else if(strnmatch( apParamStr[1], CH1, 3 ))
	{
		if(num==1)
		{
			Cal_Volt.Cal_X1=ch1_volt_disp;
			Cal_Volt.Cal_Y1=temp;
			Cal_VoltSet.Cal_X1=temp;
			Cal_VoltSet.Cal_Y1=CHAN1.volt_set;
		}
		else if(num==2)
		{
			Cal_Volt.Cal_X2=ch1_volt_disp;
			Cal_Volt.Cal_Y2=temp;
			Cal_VoltSet.Cal_X2=temp;
			Cal_VoltSet.Cal_Y2=CHAN1.volt_set;
			PARA_K[CH1_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
			PARA_K[CH1_VOLT_SET_CAL ]=Cal_Test(Cal_VoltSet);
		}
	}
	else if(strnmatch( apParamStr[1], CH3, 3 ))
	{
		if(num==1)
		{
			Cal_Volt.Cal_X1=ch3_volt_disp;
			Cal_Volt.Cal_Y1=temp;
			Cal_VoltSet.Cal_X1=temp;
			Cal_VoltSet.Cal_Y1=CHAN3.volt_set;
		}
		else if(num==2)
		{
			Cal_Volt.Cal_X2=ch3_volt_disp;
			Cal_Volt.Cal_Y2=temp;
			Cal_VoltSet.Cal_X2=temp;
			Cal_VoltSet.Cal_Y2=CHAN3.volt_set;
			PARA_K[CH3_VOLT_DISP_CAL]=Cal_Test(Cal_Volt);
			PARA_K[CH3_VOLT_SET_CAL ]=Cal_Test(Cal_VoltSet);
		}
	}

	else
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
	}
}


void  CALibration_CURRent(void)
{
	u8 num;
	u8 err=0;
	float temp;
	ERR_Flg=0;
	if( check_int_para(apParamStr[2],strlen(apParamStr[2]))==0	)
		err=1;
	if( check_para(apParamStr[3],strlen(apParamStr[3]))==0	)
		err=1;
	
	if(err)
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	num=atoi( apParamStr[2] );
	temp=atof( apParamStr[3] )	;
	if(strnmatch( apParamStr[1], CH2, 3 )	)
	{
		if(num==1)
		{
			Cal_Curr.Cal_X1=ch2_curr_disp;
			Cal_Curr.Cal_Y1=temp;

		}
		else if(num==2)
		{
			Cal_Curr.Cal_X2=ch2_curr_disp;
			Cal_Curr.Cal_Y2=temp;
			PARA_K[CH2_CURR_DISP_CAL]=Cal_Test(Cal_Curr);

		}
		else if(num==3)
		{
			Cal_CurrSet.Cal_X1=temp;
			Cal_CurrSet.Cal_Y1=CHAN2.curr_set;
		}
		else if(num==4)
		{
			Cal_CurrSet.Cal_X2=temp;
			Cal_CurrSet.Cal_Y2=CHAN2.curr_set;
			PARA_K[CH2_CURR_SET_CAL]=Cal_Test(Cal_CurrSet);
		}
	}
	else if(strnmatch( apParamStr[1], CH1, 3 ))
	{
		if(num==1)
		{
			Cal_Curr.Cal_X1=ch1_curr_disp;
			Cal_Curr.Cal_Y1=temp;
		}
		else if(num==2)
		{
			Cal_Curr.Cal_X2=ch1_curr_disp;
			Cal_Curr.Cal_Y2=temp;
			PARA_K[CH1_CURR_DISP_CAL]=Cal_Test(Cal_Curr);
		}
		else if(num==3)
		{
			Cal_CurrSet.Cal_X1=temp;
			Cal_CurrSet.Cal_Y1=CHAN1.curr_set;
		}
		else if(num==4)
		{
			Cal_CurrSet.Cal_X2=temp;
			Cal_CurrSet.Cal_Y2=CHAN1.curr_set;
			PARA_K[CH1_CURR_SET_CAL]=Cal_Test(Cal_CurrSet);
		}
	}
	else
	{
	ERR_Num=6;
	ERR_Flg=1;
	beep_on=1;
	} 
}


void ReadAll(void)
{
	u8 Result[10];
	u8 cBuf[5];
	float VoltTemp,CurrTemp;
	Reflesh_Status();
	ch1_pow_disp=ch1_curr_disp*ch1_volt_disp;  
	ch2_pow_disp=ch2_curr_disp*ch2_volt_disp;  
#ifdef CH1_RIGHT	
	sprintf(Result,"%d",REG_STAT); 
	usbtmc_response_cat( Result, strlen(Result) );	
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	VoltTemp=ch2_volt_disp;
	CurrTemp=ch2_curr_disp;
	floattostr(Result,VoltTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CurrTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,ch2_pow_disp);
	usbtmc_response_cat( Result, strlen(Result)); 
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );  
	VoltTemp=ch1_volt_disp;
	CurrTemp=ch1_curr_disp;
	floattostr(Result,VoltTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CurrTemp);

	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,ch1_pow_disp);
	usbtmc_response_cat( Result, strlen(Result)); 
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN2.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN2.curr_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN1.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN1.curr_set);
	usbtmc_response_cat( Result, strlen(Result));
#else
	sprintf(Result,"%d",REG_STAT); 
	usbtmc_response_cat( Result, strlen(Result) );	
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	VoltTemp=ch1_volt_disp;
	CurrTemp=ch1_curr_disp;
	floattostr(Result,VoltTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CurrTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,ch1_pow_disp);
	usbtmc_response_cat( Result, strlen(Result)); 
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );  
	VoltTemp=ch2_volt_disp;
	CurrTemp=ch2_curr_disp;
	floattostr(Result,VoltTemp);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CurrTemp);

	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,ch2_pow_disp);
	usbtmc_response_cat( Result, strlen(Result)); 
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN1.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN1.curr_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN2.volt_set);
	usbtmc_response_cat( Result, strlen(Result) );
	cBuf[0] = ',';
	usbtmc_response_cat( cBuf, 1 );
	floattostr(Result,CHAN2.curr_set);
	usbtmc_response_cat( Result, strlen(Result));
#endif
}

/*
void CalParam_deal(void)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(PARA_K[i].a>10||PARA_K[i].a<=0)
		{
			PARA_K[i].a=1;
			PARA_K[i].b=0;
		}
	}
}*/





