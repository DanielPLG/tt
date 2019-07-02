/*
*   usbtmc.h
*
*   Def's for USB "Test and Measurement Class" (USBTMC, USB488)
*
*   MJB  Aug. 2008
*
*/
#ifndef  _USBTMC_H_
#define  _USBTMC_H_

//--------------------------------------------------------------------------------
//       ***  SET IEEE488/USB488 DEVICE CAPABILITIES HERE  ***

#define  RL1_CAPABLE  0    // TRUE => The device supports local/remote operations
#define  DT1_CAPABLE  0    // TRUE => The device supports USBTMC_TRIGGER requests
#define  SR1_CAPABLE  0    // TRUE => The device supports USB488 service requests

#define  IEEE4882_COMPLIANT  1  // TRUE => Device is fully IEEE488.2 compliant

//--------------------------------------------------------------------------------


#define  DEVICE_CMD     'D'
#define  VENDOR_CMD     'V'

/*  Values for bRemoteLocalState variable  */
#define  RL_STATE_LOCS  'L'    /* Local state */
#define  RL_STATE_REMS  'R'    /* Remote state */
#define  RL_STATE_LWLS  'l'    /* Local with Lockout state */
#define  RL_STATE_RWLS  'r'    /* Remote with Lockout state */
#define  MAX_USBTMC_CMD_LEN  100    // USBTMC command message buffer size
#define  MAX_USBTMC_RSP_LEN  1500    // USBTMC response message buffer size

//-------------  see also usbtmc.h for TMC capabilities options  ---------------
//==============================================================================

#define  LITTLE_ENDIAN  TRUE    // SAM7 is set to little-endian mode (default)

enum  eUSBTMC_Scheduler_States
{
	USBTMC_Receiving_Cmd,       // Receiving "Program Message"
	USBTMC_Waiting_Req_Resp,    // Waiting to receive "Request for Response"
	USBTMC_Sending_Resp         // Sending Query Response
};

enum  eUSBTMCMsgID_BulkOut
{
	USBTMC_DEV_DEP_MSG_OUT = 1,
	USBTMC_REQUEST_DEV_DEP_MSG_IN = 2,
	USBTMC_VENDOR_SPECIFIC_OUT = 126,
	USBTMC_REQUEST_VENDOR_SPECIFIC_IN = 127,
	USBTMC_TRIGGER = 128
};

enum  eUSBTMCMsgID_BulkIn
{
	USBTMC_DEV_DEP_MSG_IN = 2,
	USBTMC_VENDOR_SPECIFIC_IN = 127
};


/*
*   USBTMC MESSAGE PACKET TYPES...
*   These structures define the various TMC message headers -- all have 12 bytes.
*   TransferSize doesn't include the number of bytes in the header or alignment bytes.
*/
#define  USBTMC_MSG_HEADER_LEN  12

typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar TransferAttributes;       /* Bit 0 = EOM (USBTMC spec. Table 3) */
	uchar reserved[3];              /* always 0x00 */
	} DEV_DEP_MSG_OUT_TYPE;


typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar TransferAttributes;       /* Bit 0 = EOM ... */
	                                /* Bit 1 = TermChar Enabled (USBTMC spec. Table 4) */
	uchar TermChar;                 /* TermChar (USBTMC spec. Table 4) */
	uchar reserved[2];              /* always 0x00 */
	} REQUEST_DEV_DEP_MSG_IN_TYPE;


typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar reserved[4];              /* always 0x00 */
	} VENDOR_SPECIFIC_OUT_TYPE;


typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar reserved[4];              /* always 0x00 */
	} REQUEST_VENDOR_SPECIFIC_IN_TYPE;


typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar TransferAttributes;       /* Bit 0 = EOM; Bit 1 => (USBTMC spec. Table 9) */
	uchar reserved[3];              /* always 0x00 */
	} DEV_DEP_MSG_IN_TYPE;


typedef struct
	{
	uchar MsgID;                    /* the USBTMC message type */
	uchar bTag;                     /* a transfer identifier. aka. packet number */
	uchar bTagInverse;              /* 1's complement of bTag */
	uchar reserved0;                /* always 0x00 */
	uchar TransferSize[4];          /* LSB first */
	uchar reserved[4];              /* always 0x00 */
	} VENDOR_SPECIFIC_IN_TYPE;


/*  USBTMC_GET_CAPABILITIES response message format */
typedef struct
	{
	uchar  status;
	uchar  reserved1;
	ushort usbtmcVersion;
	uchar  usbtmcIfaceCapab;
	uchar  usbtmcDeviceCapab;
	uchar  reserved6[6];
	ushort usb488Version;
	uchar  usb488IfaceCapab;
	uchar  usb488DeviceCapab;
	uchar  reserved8[8];
	} USBTMC_Capabilities_In_Type;


/*
*   USB488 Interrupt-IN packet...
*   Response to READ_STATUS_BYTE request (USBTMC class request,
*   applicable only if Device is configured for SR1 capability).
*/
typedef struct
	{
	uchar bNotify1;                     /* USB488 spec 3.4, Tables 6 & 7 */
	uchar bNotify2;                     /* USB488 spec 3.4, Tables 6 & 7 */
	} USB488_INTERRUPT_IN_PKT_TYPE;

/*____________________________________________________________________________________________*/

/*** Globals ***/

extern  const  pfnvoid  VendorDeviceRequest[];

extern  uchar  abCommandBuf[MAX_USBTMC_CMD_LEN];    // Command message buffer
extern  int    iCmdMsgLen;
extern  uchar *pbCmdMsgBuf;

extern  uchar  abResponseBuf[MAX_USBTMC_RSP_LEN];   // Response message buffer
extern  int    iRspMsgLen;
extern  uchar *pbRspMsgBuf;

extern  short  wUSBTMCstate;            // TMC scheduler state variable

extern  bool   tmc_SRQpending;          /* Service Request (SRQ) "task" is pending */
extern  bool   tmc_DeviceClear;         /* INITIATE_CLEAR class request received */
extern  bool   tmc_AbortBulkOutReq;     /* INITIATE_ABORT_BULK_OUT class request rec'd */
extern  bool   tmc_AbortBulkInReq;      /* INITIATE_ABORT_BULK_IN class request rec'd */

extern  bool   yBulkOutTferInProgress;  /* Bulk-Out Transfer is in Progress */
extern  bool   yBulkInTferInProgress;   /* Bulk-In Transfer is in Progress */
extern  bool   yUSB488_REN;             /* TRUE if Local-to-Remote transition allowed */
extern  bool   yUSB488_LLO;             /* TRUE if device is in Local Lock-Out mode */

extern  long   RxMsgBytesRemaining;     /* No. of bytes remaining in RX transfer */
extern  long   RxMsgBytesTransferred;   /* No. of bytes transferred in RX transfer */
extern  long   TxMsgBytesRemaining;     /* No. of bytes remaining in TX transfer */
extern  long   TxMsgBytesTransferred;   /* No. of bytes transferred in TX transfer */

extern  DEV_DEP_MSG_OUT_TYPE         DevDepOutHeader;
extern  REQUEST_DEV_DEP_MSG_IN_TYPE  ReqDevDepInHeader;
extern  DEV_DEP_MSG_IN_TYPE          DevDepInHeader;

#if SR1_CAPABLE
extern  USB488_INTERRUPT_IN_PKT_TYPE IntInResponsePkt;
#endif

#if RL1_CAPABLE
extern  uint8  bRemoteLocalState;             /* Remote/Local state variable */
#endif

/*____________________________________________________________________________________________*/

/*   MACROS   */

#define  CLEAR_COMMAND_MSG_BUFFER  \
			( memset( abCommandBuf, 0, sizeof(abCommandBuf) ), pbCmdMsgBuf = abCommandBuf, iCmdMsgLen = 0)

#define  CLEAR_RESPONSE_MSG_BUFFER  \
			(memset( abResponseBuf, 0, sizeof(abResponseBuf) ), pbRspMsgBuf = abResponseBuf, iRspMsgLen = 0)


/*____________________________________________________________________________________________*/

/*   usbtmc.c Function prototypes  */
void  wordToHexStr( uint16 wVal, char *pcResult );
void  longToDecimalStr( int32 lVal, char *pcResult, uchar bFieldSize );
void  usbtmc_scheduler( void );
void  usbtmc_cmd_msg_handler( void );
void  usbtmc_rsp_msg_handler( void );
void  usbtmc_initialise( void );
void  usbtmc_task_abort_bulk_in( void );
void  usbtmc_command_cat( uchar *pbSource, short wCount );
void  usbtmc_response_cat( u8 *pbSource, short wCount );
void  toLittleEndian( uint32 value, uchar *pDestin );
void  send_SRQ_via_Intrpt_In( void );
void  init_rem_loc_state( void );
void  update_rem_loc_state( void );
void  go_to_remote( void );
void  go_to_local( void );
bool  in_remote( void );
char  hexitoc( short wDigit );
uint16  hexatoi( char * s );
uint8  hexctobin( char c );
bool  usbEndpointTxBuffReady( uint8 bEndp );
/*___________________________________________________________________________*\
*
*   Meanings of Debug Flags, bits in glDebugFlags word:

 Bit	Indicates ___________________________________________________________

 16		USBTMC_DEV_DEP_MSG_OUT message (header packet) received        [info]
 17		Response message handler, usbtmc_rsp_msg_handler(), entered    [info]
 18		Response formatter buffered a complete rsp message             [info]
 19		REQUEST_DEV_DEP_MSG_IN request received                        [info]

 20		Response message header packet written to Bulk-In FIFO         [info]
 21		Response message sent (Transfer complete)                      [info]
 22		Service Request (SRQ) packet TX'd on Interrupt-IN endpoint     [info]
 23		reserved                                                       [info]

 24		USBTMC_INITIATE_ABORT_BULK_IN (TMC Class Request) rx'd         [info]
 25		USBTMC_REN_CONTROL request received                            [info]
 26		USBTMC_READ_STATUS_BYTE request received                       [info]
 27		USBTMC_INITIATE_CLEAR request received                         [info]

 28		Query Error - Response requested but no query received        [Error]
 29		Query Error - <PRG MSG> rec'd; expecting REQ_DEV_DEP_MSG_IN   [Error]
 30		Message length error -- Rx packet shorter than expected       [Error]
 31		Message exchange protocol error or buffer overflow            [Error]

\*___________________________________________________________________________*/

#endif  /* _USBTMC_H_ */
