#ifndef   _SCPI_H
#define   _SCPI_H
 #include "Main.h"
#define   Serial_No_Long  14

 typedef struct _scpi_keyword
{
	const u8 shortWord[6];
	const u8 longWord[16];
}SCPI_KEYWORD;
 typedef struct _scpi_ERRO
{
	 u8 NO;
	 char* ERR_INFO; 
}SCPI_ERR_INFO;
extern const u8 com_cmd[];


extern const SCPI_KEYWORD keywords[];
extern const SCPI_ERR_INFO ERR[];
extern void SCPI_cmd_cls(void);
extern u8 SCPI_decode(u8 *buf);
void INSTrument(void) ;
void SAV(void);
void RCL(void);
void qry_INSTrument(void);
void MEASure_CURRent(void);
void MEAsure_VOLTage(void);
void MEASure_POWEr(void);
void CH1_CURRent(void);
void CH2_CURRent(void);
void Qry_CH1_CURRent(void);
void Qry_CH2_CURRent(void);
void CH1_VOLTage(void);
void CH2_VOLTage(void);
void Qry_CH1_VOLTage(void);
void Qry_CH2_VOLTage(void);
void OUTPut_STATe(void);
void OUTPut_TRACk(void);

void  SYSTem_ERRor(void);
void  SYSTem_VERSion(void);
void  SYSTem_STATus(void);
void  CALibration_VOLTage(void);
void  CALibration_CURRent(void);
void SCPI_IDN(void);
void dmm_set_scpi_Interface_type(unsigned char state);
unsigned char dmm_get_scpi_Interface_type(void);
#endif