/*
*   usbreq.h
*
*   Def's for USB device communications - "Device Request" layer,
*   conforming to USB 2.0 specification, Chapter 9.
*
*   Customised for USB Test & Measurement Class (USBTMC).
*/
#ifndef  USBREQ_H_
#define  USBREQ_H_


#define  USB_VENDOR_CODE    ((uint16) 0x03EB)   // Must match *.inf file (if any)
#define  USB_PRODUCT_CODE   ((uint16) 0x6166)   // Must match *.inf file (if any)
#define  USB_DEVICE_VERN    ((uint16) 0x0101)   // Device hardware version #

#define  DEVICE_CLASS_USBTMC      (0xFE)
#define  DEVICE_SUBCLASS_488      (0x03)

#define  NUMBER_STD_REQ       13
#define  NUMBER_VENDOR_REQ    16

/* USBTMC class-specific bRequest values */
#define  USBTMC_INITIATE_ABORT_BULK_OUT      1
#define  USBTMC_CHECK_ABORT_BULK_OUT_STATUS  2
#define  USBTMC_INITIATE_ABORT_BULK_IN       3
#define  USBTMC_CHECK_ABORT_BULK_IN_STATUS   4
#define  USBTMC_INITIATE_CLEAR               5
#define  USBTMC_CHECK_CLEAR_STATUS           6
#define  USBTMC_GET_CAPABILITIES             7
#define  USBTMC_INDICATOR_PULSE             64
#define  USBTMC_READ_STATUS_BYTE           128
#define  USBTMC_REN_CONTROL                160
#define  USBTMC_GO_TO_LOCAL                161
#define  USBTMC_LOCAL_LOCKOUT              162

/* USBTMC class requests -- response[0] values */
#define  USBTMC_STATUS_SUCCESS                   0x01
#define  USBTMC_STATUS_PENDING                   0x02
#define  USBTMC_STATUS_INTERRUPT_IN_BUSY         0x20
#define  USBTMC_STATUS_FAILED                    0x80
#define  USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS  0x81
#define  USBTMC_STATUS_SPLIT_NOT_IN_PROGRESS     0x82
#define  USBTMC_STATUS_SPLIT_IN_PROGRESS         0x83


/* CONFIGURATION DESCRIPTOR */
#define NB_INTERFACE          1
#define CONF_NB               1
#define CONF_INDEX            0
#define CONF_ATTRIBUTES       0x80      /* Bit7 bus-powered; Bit6 self-powered; Bit5 RWU enab */
#define MAX_POWER             1         /* Maximum VBUS current drain = MAX_POWER x 2 (mA) */

/* INTERFACE 0 DESCRIPTOR */
#define INTERFACE0_ID          0
#define ALTERNATE0             0
#define INTERFACE0_CLASS       DEVICE_CLASS_USBTMC
#define INTERFACE0_SUB_CLASS   DEVICE_SUBCLASS_488
#define INTERFACE0_PROTOCOL    1
#define INTERFACE0_INDEX       0

/* ENDPOINT 1 DESCRIPTOR -- Bulk-Out (RX from host) */
#define ENDPOINT_ID_1         BULK_OUT_ENDP
#define EP_ATTRIBUTES_1       0x02
#define EP_SIZE_1             BULK_MAX_PKT_LEN
#define EP_INTERVAL_1         0

/* ENDPOINT 2 DESCRIPTOR -- Bulk-In (TX to host) */
#define ENDPOINT_ID_2         (0x80 | BULK_IN_ENDP)
#define EP_ATTRIBUTES_2       0x02
#define EP_SIZE_2             BULK_MAX_PKT_LEN
#define EP_INTERVAL_2         0

/* ENDPOINT 3 DESCRIPTOR -- Interrupt-In (TX to host) */
#define ENDPOINT_ID_3         (0x80 | INTR_IN_ENDP)
#define EP_ATTRIBUTES_3       0x03
#define EP_SIZE_3             INTR_MAX_PKT_LEN
#define EP_INTERVAL_3         10                /* Polling interval (ms) */


/**************
*   String Descriptors...
*/
#define UNICODE_LANGUAGE_STR_ID  (0)    /* Index values */
#define MANUFACTURER_STR_ID      (1)
#define PRODUCT_NAME_STR_ID      (2)
#define SERIAL_NUM_STR_ID        (3)
#define UNICODE_ENGLISH     (0x0409)    /* US_English (Ref: USB_LANGIDs.pdf) */


/***********
*   Control endpoint packet structures
*/
typedef struct _device_request       /* SETUP packet data (8 bytes) */
{
	uchar   bmRequestType;
	uchar   bRequest;
	uint16  wValue;
	uint16  wIndex;
	uint16  wLength;
} DEVICE_REQUEST;

#define MAX_CONTROLDATA_SIZE    64   /* Maximum length of CTRL-IN or CTRL-OUT Transfer */

typedef struct _control_xfer
{
	DEVICE_REQUEST DeviceRequest;    /* SETUP packet data */
	uint16  wLength;                 /* Ctrl-OUT Transfer size (from SETUP request) */
	uint16  wCount;                  /* Bytes received in Ctrl-OUT transfer (cumulative) */
	uchar   * pData;                 /* Pointer to dataBuffer[] for Ctrl-OUT transfer */
	uchar   dataBuffer[MAX_CONTROLDATA_SIZE];   /* Buffer for Ctrl-OUT data */
} CONTROL_XFER;

extern  CONTROL_XFER  ControlData;


/************************************************************************** */
/* Control endpoint handlers */
/************************************************************************** */

void    setup_req_handler( void );
void    control_out_handler( void );
void    control_dispatcher( void );
void    control_transmit( uchar * buf, uint16 wLen );

/************************************************************************** */
/* USB ch.9 standard device requests */
/************************************************************************** */

extern  const pfnvoid  StandardDeviceRequest[];

void    get_status(void);
void    clear_feature(void);
void    set_feature(void);
void    set_address(void);
void    get_descriptor(void);
void    get_configuration(void);
void    set_configuration(void);
void    get_interface(void);
void    set_interface(void);
void    req_undefined(void);

/************************************************************************** */
/* USB ch.9 class-specific device requests (USB TMC) */
/************************************************************************** */

void  UsbTMCInitiateAbortBulkOut( void );
void  UsbTMCCheckAbortBulkOutStatus( void );
void  UsbTMCInitiateAbortBulkIn( void );
void  UsbTMCCheckAbortBulkInStatus( void );
void  UsbTMCInitiateClear( void );
void  UsbTMCCheckClearStatus( void );
void  UsbTMCGetCapabilities( void );
void  UsbTMCIndicatorPulse( void );
void  UsbTMC488ReadStatusByte( void );
void  UsbTMC488RENcontrol( void );
void  UsbTMC488GoToLocal( void );
void  UsbTMC488LocalLockOut( void );

/************************************************************************** */
/* USB ch.9 utility functions */
/************************************************************************** */

uint32  LittleEndianToLong( uchar * pSource );
void    toLittleEndian( uint32 value, uchar * pDestin );


#endif  /* USBREQ_H_ */
