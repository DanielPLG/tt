/****************************************Copyright (c)****************************************************
**
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The uCOSII2.91+UCGUI3.90A application function
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
#include "common.h"
#include "GUI.H"
#include "RTC_Time.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "TouchPanel.h"
#include <stdio.h>
//#define   CONFIG_JTAG    1
#define IWDG_WriteAccess_Enable     ((u16)0x5555)
//#define CONFIG_JTAG      1
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskStartStk[APP_TASK_START_STK_SIZE];
extern u8 key_ext_flg;
u8 SCPI_SER_FLG,SCPI_SER_SET;
u8 cur_page;
u8 ERR_Num;
u8 ERR_Flg;
u8 CHAN_STAT;
u8 CHAN_SEL=0;
u8 recall_SET_FLG=0;

u8 CH1_SET_FLG,CH2_SET_FLG;
u8 SCPI_PARA_FLG,SCPI_PARA_SET;
u8 CH3_SET_FLG=0;

u8 VOLT_REFLESH;
u8 CURR_REFLESH;
u8 SCPI_CH1_SET,SCPI_CH2_SET,SCPI_CH3_SET,SCPI_OVP,SCPI_OCP;

u8 file_cur;
u8 CH1Mode_flg,CH1Mode_reflesh;
u8  ext_trig_flg=1;
u8 OTPMode_flg=0;
u8 OTPMode_reflesh=0;
u8 CH2Mode_flg,CH2Mode_reflesh;
u8 CH3Mode_flg,CH3Mode_reflesh;

u8  CH2CV_mode_enable=0;
u8  recall_count=0;

u8 main_info_flg;
u8 info_stat;
unsigned char  DsipBuffer[NUM_OF_DISP_BUF];
int g_BootTimes;
unsigned char cal_mode_enable=0;

extern u8 BeeperFlg;
u8 timer_count_stat;

u8 rcl_inprogess;
u8 screen_reverse;

extern u8  ovp_flash_flg,ovp_flg;


STORE_PARA  para_factory;

/* Private function prototypes -----------------------------------------------*/
#if (OS_VIEW_MODULE == DEF_ENABLED)
extern void  App_OSViewTaskCreate   (void);
#endif

static  void  App_TaskCreate        (void);
static  void  App_TaskStart         (void       *p_arg);
extern  void  App_UCGUI_TaskCreate  (void);
static  void  GPIO_Configuration    (void);
void DELAY_MS1(int time);
void  BEEP_RING( int time )
{
    int i;
    //for(i=0;i<time;i++)

    if(BeeperFlg)
    {
        key_ext_flg=1;
        BEEP_ON;
        DELAY_MS1(5);
        BEEP_OFF;
        //DELAY_MS1(10);
    }
    //void beeper(void)

    //GPIO_SetBits(GPIOD,GPIO_Pin_6);
    // delay_ms(10);
    // GPIO_ResetBits(GPIOD,GPIO_Pin_6);


}
void  BEEP_RING3( int time )
{
    int i;
    //for(i=0;i<time;i++)

    if(BeeperFlg)
    {
        key_ext_flg=1;
        BEEP_ON;
        DELAY_MS1(5);
        BEEP_OFF;
        //DELAY_MS1(10);
    }
    //void beeper(void)

    //GPIO_SetBits(GPIOD,GPIO_Pin_6);
    // delay_ms(10);
    // GPIO_ResetBits(GPIOD,GPIO_Pin_6);


}
void  BEEP_RING1( int time )
{
    int i;
    //for(i=0;i<time;i++)
    //if(BeeperFlg)
    {
        BEEP_ON;
        DELAY_MS1(5);
        BEEP_OFF;
    }



}
void Select_recall(uint8_t channel)
{
    unsigned  char ledreg;
    switch (channel)
    {

        case 0:
            LED1_M1(0);
            LED1_M2(0);
            LED1_M3(0);
            LED1_M4(0);
            LED1_M5(0);


            break;
        case 1:
            LED1_M1(1);
            LED1_M2(0);
            LED1_M3(0);
            LED1_M4(0);
            LED1_M5(0);
            break;
        case 2:
            LED1_M1(0);
            LED1_M2(1);
            LED1_M3(0);
            LED1_M4(0);
            LED1_M5(0);

            break;
        case 3:
            LED1_M1(0);
            LED1_M2(0);
            LED1_M3(1);
            LED1_M4(0);
            LED1_M5(0);
            break;
        case 4:
            LED1_M1(0);
            LED1_M2(0);
            LED1_M3(0);
            LED1_M4(1);
            LED1_M5(0);
            break;
        case 5:
            LED1_M1(0);
            LED1_M2(0);
            LED1_M3(0);
            LED1_M4(0);
            LED1_M5(1);
            break;
        default:

            break;
    }

}
extern u8 ch3_ChannelSel;
void Select_CH3(uint8_t channel);

void CH3_value_set(float value)
{
    if(value>1.76&&value<1.82)
    {
        CHAN3.volt_set=1.8;
        ch3_ChannelSel=0;
        para_store.ch3_Sel=ch3_ChannelSel;
        Select_CH3(ch3_ChannelSel);
    }
    else if(value>2.48&&value<2.52)
    {
        ch3_ChannelSel=1;
        para_store.ch3_Sel=ch3_ChannelSel;
        Select_CH3(ch3_ChannelSel);

    }
    else if(value>3.28&&value<3.32)
    {
        ch3_ChannelSel=2;
        para_store.ch3_Sel=ch3_ChannelSel;
        Select_CH3(ch3_ChannelSel);

    }
    else if(value>4.98&&value<5.02)
    {
        ch3_ChannelSel=3;
        para_store.ch3_Sel=ch3_ChannelSel;
        Select_CH3(ch3_ChannelSel);

    }
    else
    {
        LED_CH3_1V8(0);
        LED_CH3_2V5(0);
        LED_CH3_3V3(0);
        LED_CH3_5V0(0);

    }

}
void Select_CH3(uint8_t channel)
{
    unsigned  char ledreg;
    switch (channel)
    {

        case 0:

            CHAN3.volt_set=1.8;
            CH3_SET_FLG=1;
            LED_CH3_1V8(1);
            LED_CH3_2V5(0);
            LED_CH3_3V3(0);
            LED_CH3_5V0(0);
            ledreg=0x0c;
            ledreg=0x07;
            break;
        case 1:
            //CH3POWER_1v8_OFF;
            //CH3POWER_2v5_ON;
            //CH3POWER_3v3_OFF;
            //CH3POWER_5v0_OFF;
            CHAN3.volt_set=2.5;
            CH3_SET_FLG=1;
            LED_CH3_1V8(0);
            LED_CH3_2V5(1);
            LED_CH3_3V3(0);
            LED_CH3_5V0(0);

            ledreg=0x0a;
            //WriteAll_1621_CH1(25,&ledreg,1);
            ledreg=0x07;
            //  WriteAll_1621_CH1(31,&ledreg,1);
            break;
        case 2:
            //CH3POWER_1v8_OFF;
            //  CH3POWER_2v5_OFF;
            //CH3POWER_3v3_ON;
            //CH3POWER_5v0_OFF;
            CHAN3.volt_set=3.3;
            CH3_SET_FLG=1;

            ledreg=0x09;
            //WriteAll_1621_CH1(25,&ledreg,1);
            ledreg=0x07;
            //WriteAll_1621_CH1(31,&ledreg,1);
            LED_CH3_1V8(0);
            LED_CH3_2V5(0);
            LED_CH3_3V3(1);
            LED_CH3_5V0(0);
            break;
        case 3:
            //CH3POWER_1v8_OFF;
            //  CH3POWER_2v5_OFF;
            //CH3POWER_3v3_OFF;
            //CH3POWER_5v0_ON;
            CHAN3.volt_set=5;
            CH3_SET_FLG=1;

            LED_CH3_1V8(0);
            LED_CH3_2V5(0);
            LED_CH3_3V3(0);
            LED_CH3_5V0(1);

            ledreg=0x08;
            //WriteAll_1621_CH1(25,&ledreg,1);
            ledreg=0x0f;
            //WriteAll_1621_CH1(31,&ledreg,1);
            break;
        case 4:
            //CH3POWER_1v8_OFF;
            //CH3POWER_2v5_OFF;
            CH3POWER_3v3_OFF;
            CH3POWER_5v0_OFF;
            break;

        default:
            //CH3POWER_1v8_OFF;
            //CH3POWER_2v5_OFF;
            CH3POWER_3v3_OFF;
            CH3POWER_5v0_OFF;
            break;
    }
    ledreg=0x08;
    WriteAll_1621_CH1(25,&ledreg,1);

    GPIO_ResetBits(GPIOB, GPIO_Pin_11);


}
uint8_t key_scan(void)
{
    KH1_scan;
    if (!ReadKV1)
    {
        return KEY_CH2;
    }
    if (!ReadKV2)
    {
        return KEY_SETVIEW;
    }
    if (!ReadKV3)
    {
        return KEY_CH1 ;
    }
    if (!ReadKV4)
    {
        return KEY_M4;
    }
    KH2_scan;
    if (!ReadKV1)
    {
        return  KEY_ALL_ON;
    }
    if (!ReadKV2)
    {
        return KEY_BEEPER;
    }
    if (!ReadKV3)
    {
        return KEY_M5;
    }
    if (!ReadKV4)
    {
        return KEY_M3 ;
    }

    KH3_scan;
    if (!ReadKV1)
    {
        return  KEY_OCP ;
    }
    if (!ReadKV2)
    {
        return KEY_LOCK ;
    }
    if (!ReadKV3)
    {
        return  KEY_OVP  ;
    }
    if (!ReadKV4)
    {
        return KEY_M2 ;
    }
    KH4_scan;
    if (!ReadKV1)
    {
        return KEY_FINE_CURRENT ;
    }
    if (!ReadKV2)
    {
        return KEY_CH3 ;
    }
    if (!ReadKV3)
    {
        return KEY_FINE ;
    }
    if (!ReadKV4)
    {
        return KEY_M1 ;
    }

    return 0;
}

void DELAY_MS1(int time)
{
    u16 tt=0x1fff;
    while(time--)
    {
        while(tt--);
    }
}
void DELAY_MS(int time)
{
#ifdef  AD7792
    u16 tt=0x1ff;
#else
    u16 tt=0xff;
#endif
    //u16 tt=0x5ff;
    // u16 tt=0xff;
    while(time--)
    {
        while(tt--);
    }
}
void DELAY_MS3(int time)
{
    u16 tt=0x5ff;
    //u16 tt=0xff;-
    while(time--)
    {
        // if(key_ext_flg)
        //return;
        while(tt--)
        {
            // if(key_ext_flg)
            //return;
        }
    }
}

void DELAY_MS2(int time)
{
    u16 tt=0x5ff;
    //u16 tt=0xff;-
    while(time--)
    {
        // if(key_ext_flg)
        //return;
        while(tt--)
        {
            // if(key_ext_flg)
            //return;
        }
    }
}
Cal_K Cal_Test(cal_data val)
{
    Cal_K temp;
    if((val.Cal_Y2-val.Cal_Y1)>=0)
        temp.a=(val.Cal_Y2-val.Cal_Y1)/(val.Cal_X2-val.Cal_X1);
    else
        temp.a=(val.Cal_Y1-val.Cal_Y2)/(val.Cal_X1-val.Cal_X2);
    temp.b= val.Cal_Y2-temp.a*val.Cal_X2;
    return temp;
}
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
u16 value;
extern unsigned int  advalue;
extern u8 ch3_ChannelSel;
extern u8 usb_reflesh,usb_flg;
u8 adc_id;
void maintest(void)
{
    LED1_M1(1);
    LED1_M2(1);
    LED1_M3(1);
    LED1_M4(1);
    LED1_M5(1);

    DELAY_MS1(10);
    LED1_M1(0);
    LED1_M2(0);
    LED1_M3(0);
    LED1_M4(0);
    LED1_M5(0);

    DELAY_MS1(10);
}

void main(void)
{

    CPU_INT08U  os_err;

    RCC_Configuration();


    GPIO_Configuration();
    NVIC_Configuration();
    Set_USBClock();
    USB_Init();
    USB_ENABLE;
    CH3Mode_flg=1;
    usb_flg=1;
    para_start_set();
    FLASH_Unlock();
    RTC_Init();
    TIMER_START(1);
    //EXTI_Configuration1();
    USART_Configuration();
#ifndef CONFIG_JTAG
    RCC->APB2ENR |= 0x00000001;
    AFIO->MAPR = (0x00FFFFFF & AFIO->MAPR)|0x04000000;          //关闭JTAG
#endif

    AD7562_io(0);


    PARA_Read(CAL_ADDRESS,&PARA_K[0], sizeof(PARA_K));
    CalParam_deal();
    printf("*  UTP3000C ! ^_^  *\r\n");
    PARA_Read(Delault_Set_ADDRESS,&para_store, sizeof(para_store));
    if(para_store.flg==0xaa)
    {
        Rcl_set(para_store);
    }
    else
    {
        para_store.ch3_Sel=0;
        BeeperFlg=0;
        //ch3_ChannelSel=para_store.ch3_Sel;
    }
    ch3_ChannelSel=para_store.ch3_Sel;
    if(ch3_ChannelSel>4)
    {
        ch3_ChannelSel=0;
        para_store.ch3_Sel=ch3_ChannelSel;
    }
    usbtmc_initialise();
    PowerOn_Test();
    //DELAY_MS1(120);
    HT1621_Test();
    LED_ON_OFF(0);
    LED_OVP(0);
    LED_OCP(0);
    LED1_M1(0);
    LED1_M2(0);
    LED1_M3(0);
    LED1_M4(0);
    LED1_M5(0);
    LED_CH3(0);

    //LED_CH1_CV_ON ;
    //LED_CH2_CV_ON ;
    //LED_SETVIEW(0);
    LED_SETVIEW(0);
    LED_CH1_CV_OFF ;
    LED_CH2_CV_OFF;
    LED_SER_CH1(1);
    LED_PARA_CH2(0);

    init_advalue();
    DA7562_CLK(0);
    func_set(CHAN1.volt_set,1,CH1_SEL);
    func_set(CHAN1.curr_set,0,CH1_SEL);
    ch2DA7562_CLK(0);
    func_set((CHAN2.volt_set),1,CH2_SEL);
    func_set(CHAN2.curr_set,0,CH2_SEL);
    func_set((CHAN2.volt_set),1,CH2_SEL);
    func_set(CHAN2.curr_set,0,CH2_SEL);
    CH3_ad_da_INIT();
    ch3_da_test();
    // maintest();
    CH4_ON_DELAY(0);
    CH1POWER_OFF;
    DELAY_MS1(10);
    //CH3_AD_SPI_Init();
    ch3_test_disp();
    //while(1)
    {
        //ch3_da_test();
        value=CH3_ADS7040_Write_Read(0xffff);
        DELAY_MS1(10);
    }
    //ADS7040_io();
    DELAY_MS2(20);
    ADS1118_init()  ;
    if(para_store.flg==0xaa)
    {
        if(para_store.mode_flg==0x02)
        {
            g_stat.PARA_FLG=0xff;
            deal_PARA();
        }
        else if(para_store.mode_flg==0x03)
        {
            g_stat.SER_FLG=0xff;
            deal_SER();
        }
    }

    DELAY_MS2(20);

    CPU_IntDis();                    /* Disable all ints until we are ready to accept them.  */

    OSInit();                        /* Initialize "uC/OS-II, The Real-Time Kernel".         */


    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif

    OSStart();
    while(1)
    {
//  GPIO_Configuration();
    }
}




/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskStart (void *p_arg)
{

    (void)p_arg;

    /***************  Init hardware ***************/
    OS_CPU_SysTickInit();
    //  TP_Init();
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                            /* Determine CPU capacity.                              */
#endif
    App_TaskCreate();                                        /* Create application tasks.                            */
    for(;;)
    {
        OSTaskSuspend(OS_PRIO_SELF);
        //    OSTimeDlyHMSM(0, 1, 0, 0);                           /* Delay One minute */
    }
}


/*
*********************************************************************************************************
*                                            App_TaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
#if (OS_VIEW_MODULE == DEF_ENABLED)
    App_OSViewTaskCreate();
#endif

    App_UCGUI_TaskCreate();

}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure GPIO Pin
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void HT1621_IO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 |  GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void LedDisp_IO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|  GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4  |GPIO_Pin_8 |GPIO_Pin_9  |GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 |  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|  GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void Key_IO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|  GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_5 |GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_15  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0   ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 |  GPIO_Pin_2 | GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    USB_DISABLE;
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3  |GPIO_Pin_4  | GPIO_Pin_8  |GPIO_Pin_9  |GPIO_Pin_13  | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 |GPIO_Pin_1| GPIO_Pin_7 | GPIO_Pin_8 |  GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 |GPIO_Pin_5 |  GPIO_Pin_6  |GPIO_Pin_8 | GPIO_Pin_9 |  GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOC, GPIO_Pin_5 );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  |GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2|GPIO_Pin_6|GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_6);



    //KLINE DECTECT
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_11  ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    /* GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; ;
     GPIO_Init(GPIOD, &GPIO_InitStructure); */
    TP_CS(1);
    DA7562_CS(1);
    // USB_ENABLE;

    //  GPIO_SetBits(GPIOA,GPIO_Pin_4);
    CH1POWER_OFF;
    CH2POWER_OFF;
    // CH3POWER_OFF;
    BEEP_OFF;
    KH5_scan;
    HT1621_IO_init();
    Ht1621_CS0(1);
    LedDisp_IO_init();
    Key_IO_init();
    KH1_scan ;
    // CH3POWER_1v8_OFF;
    //CH3POWER_2v5_OFF;
    CH3POWER_3v3_OFF;
    CH3POWER_5v0_OFF;
    LED_CH3(0);
    if (!ReadKV1)
    {
        while(1)
        {
            LED_CH3(1);
        }
    }
    if (!ReadKV2)
    {
        cal_mode_enable=1;
    }
    // GPIO_SetBits(GPIOA,GPIO_Pin_3);
}

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}

    return ch;


    return ch;
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
void IWDG_Configuration(void)
{
    /* 写入0x5555,用于允许狗狗寄存器写入功能 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
    IWDG_SetPrescaler(0x00);

    /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
    IWDG_SetReload(1);

    /* 喂狗*/
    IWDG_ReloadCounter();

    /* 使能狗狗*/
    IWDG_Enable();
}




