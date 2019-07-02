#include "scpi.h"
#include "USBTMC.h"
#include "USB488.h"
#include "usbreq.h"
#include "comm.h"
#include <string.h>

#include "..\USER\usb\usb_lib\inc\usb_regs.h"
#define ENDP0   ((u8)0)
#define ENDP1   ((u8)1)
#define ENDP2   ((u8)2)
#define USART_REC_LEN  			200 
void Scpi_write_data_usart(uint8_t* Buf, uint32_t Len);
// Maximum message data payload in first packet (header pkt) of transfer
#define  FIRST_MAX_PAYLOAD_LEN  (BULK_MAX_PKT_LEN - USBTMC_MSG_HEADER_LEN)

bool  decodeCommand( uchar abCmdBuf[] );
extern u32 count_out;
extern u32 count_in;
extern u8 TxBusy;
extern u8 USART_RX_BUF[];
extern u16 USART_RX_STA;
extern u8  receive_stat;
extern u8 Scpi_Measure_Qury;
extern u8 Virtual_Com_Port_StringSerial[30];
//** Globals **
short  wUSBTMCstate;            // TMC scheduler state variable (global for debug)

u8    zIDN_string[] = "UNI-T Technologies,UTP3000C,";


uchar  abCommandBuf[MAX_USBTMC_CMD_LEN];    // Command message buffer
int    iCmdMsgLen;
uchar *pbCmdMsgBuf;
CONTROL_XFER    ControlData; 
uchar  abResponseBuf[MAX_USBTMC_RSP_LEN];   // Response message buffer
int    iRspMsgLen;
uchar *pbRspMsgBuf;

bool   tmc_SRQpending;           // Service Request (SRQ) "task" is pending
bool   tmc_DeviceClear;          // INITIATE_CLEAR class request received
bool   tmc_AbortBulkOutReq;      // INITIATE_ABORT_BULK_OUT class request rec'd
bool   tmc_AbortBulkInReq;       // INITIATE_ABORT_BULK_IN class request rec'd

bool   yBulkOutTferInProgress;   // Bulk-Out Transfer is in Progress
bool   yBulkInTferInProgress;    // Bulk-In Transfer is in Progress
bool   yUSB488_REN;              // Local-to-Remote transition allowed
bool   yUSB488_LLO;              // Device has Local Lock-Out asserted

long   RxMsgBytesRemaining;      // No. of bytes remaining in RX transfer
long   RxMsgBytesTransferred;    // No. of bytes transferred in RX transfer
long   TxMsgBytesRemaining;      // No. of bytes remaining in TX transfer
long   TxMsgBytesTransferred;    // No. of bytes transferred in TX transfer

DEV_DEP_MSG_OUT_TYPE          DevDepOutHeader;
REQUEST_DEV_DEP_MSG_IN_TYPE   ReqDevDepInHeader;
DEV_DEP_MSG_IN_TYPE           DevDepInHeader;
 
uint8   gbSTB = 0;            // Status Byte register (STB)
uint8   gbSRE = 0;            // Service Request Enable (SRE) register (not used)
uint16  gwESE = 0x0080;       // Event Status Enable (ESE) register
uint16  gwESR = 0x0080;       // Event Status Register (ESR)

// Device application-specific data...

int16   iwaConfigParam[4];    // Device configuration parameters (array of 4)
uint16  wButtonHitCounter;    // Quantity returned by COUNT:READ? query
bool    yIndLEDstate;         // State of "Indicator" LED (1 => LED ON)
bool    gb5msecTaskReq;
bool    gb50mSecTaskReq;
bool    gb500msecTaskReq;
uint32  glDebugFlags;

static  uint32  lButtonStateReg;
static  uint32  lButtonHitReg;
static  int     iIndicatorPulseTimer;
static  bool    yIndPulseExpired;

static  uint8   bSelfTestEcode;                 // Error code from POR self-test routine
static  uint8   bParamCount;                    // Number of command params
char  * apParamStr[MAX_CMD_PARAMS];     // Array of parameter strings
 
static  char  * apcommd[5];
// IEEE488/SCPI common command definitions ...

	
 
static  uchar  BulkOUTpkt[BULK_MAX_PKT_LEN];    // Packet buffer (used for RX)
static  uchar *pBulkOUTpkt_data;                // Ptr to start of pkt msg data in BulkOUTpkt
static  uchar  BulkINpkt[BULK_MAX_PKT_LEN];     // Packet buffer (used for TX
static  uchar *pBulkINpkt_data;                 // Ptr to start of pkt msg data in BulkINpkt

static  bool   yCmdBufOverflow;                 // Message length exceeds buffer size
static  bool   yBusTriggerRecd;                 // Signal to execute trigger_control
static  bool   yRequestBulkInMsg;               // Request for Query Response received
static  bool   yRespMsgSent;                    // Response message has been transmitted
//static  bool   yRespen;                   

static  int    iMaxRespMsgLen;                  // Maximum size (chars) of Response Msg
static  bool   yFirstPktRxTransfer;             // Bulk Out (Rx) pkt is first in transfer
static  bool   yFirstPktTxTransfer;             // Bulk In (Tx) pkt is first in transfer
static  bool   yEndOfMessage;                   // Received transfer with EOM bit set
static  u8   yDeviceMsgHdrRxd;                // Device-dependent msg Header rec'd
u8   yProgMessRecd;                   // Complete <PROGRAM MESSAGE> received
extern char ptr_serial[30];

/*
*   IEEE "Indicator Pulse" function:
*   Returns TRUE if the 0.5 second timer is expired.
*/
bool  expiredIndPulseTimer( void )
{
	return  yIndPulseExpired;
}
void Clear_Command_Buffer(void)
{
	u8 i;
	memset( abCommandBuf, 0, sizeof(abCommandBuf) );
	pbCmdMsgBuf = abCommandBuf;
	iCmdMsgLen = 0;
}
void Clear_Response_Buffer(void)
{
	memset( abResponseBuf, 0, sizeof(abResponseBuf) );
	pbRspMsgBuf = abResponseBuf;
	iRspMsgLen = 0;
}  
/*_______________________________________________________________________________________*\
*
*   Control Transfer SETUP request handler...  called by usb_service().
*
*   Unloads a SETUP packet (8 bytes) received from the host via endpoint 0.
*   The routine handles the SETUP stage of a Control Transfer (Device Request),
*   which may have a Data-In stage or Data-Out stage (or nothing) to follow.
*
*   If the request has a Data-Out stage to follow, Data-Out packet(s) are unloaded
*   by the function control_out_handler(), called from the USB service routine.
*   If the request has a Data-In stage to follow, Data-In packet(s) are sent to the
*   host by the respective device request function, called by control_dispatcher().
*   If there is no DATA stage following the SETUP stage, the request is processed
*   immediately by a call to the control_dispatcher().
*/
void  setup_req_handler( void )
{
	//usbReadEndpointData( SETUP_ENDP, (uchar *) &(ControlData.DeviceRequest), 8 );
	PMAToUserBufferCopy((u8 *) &(ControlData.DeviceRequest), GetEPRxAddr(ENDP0), 8);
	// Packet format is little-endian ... data structure may be big-endian
	ControlData.DeviceRequest.wValue  = LSB_MSB(ControlData.DeviceRequest.wValue);
	ControlData.DeviceRequest.wIndex  = LSB_MSB(ControlData.DeviceRequest.wIndex);
	ControlData.DeviceRequest.wLength = LSB_MSB(ControlData.DeviceRequest.wLength);

	ControlData.wLength = ControlData.DeviceRequest.wLength;  // Transfer size
	ControlData.wCount = 0;         // Number of bytes transferred in DATA stage
	ControlData.pData = &ControlData.dataBuffer[0];

	//USBflags.b.control_in = FALSE;
	//USBflags.b.control_out = FALSE;
}

/*
*   This routine updates the Status Summary bits in the IEEE488 status reporting system.
*   Bit STB:ESB is set if any bit in ESR is set AND the corresponding bit in ESE is set.
*   Bit STB:MSS is set if any bit in STB is set, other than STB:MSS.
*
*   If the device is SR1 capable (i.e. it supports Service Requests), then...
*   if a 0-to-1 transition has occurred in any bit of the STB register (except bit-6),
*   AND the corresponding bit in the SRE (Service Request Enable) register is set, then
*   the RQS bit is set and a "Service Request" is signalled.
*   NB:  Service Request capability has not been fully tested! (Example device is SR0.)
*/
void  device_status_update( void )
{
#if SR1_CAPABLE
	static   uint8  bLastSTB = 0;
#endif

	if ( gwESR & gwESE ) 
		SET_BIT( gbSTB, STB_ESB );
	else  
		CLEAR_BIT( gbSTB, STB_ESB );

	if ( gbSTB & ~STB_MSS ) 
		SET_BIT( gbSTB, STB_MSS );
	else  
		CLEAR_BIT( gbSTB, STB_MSS );

#if SR1_CAPABLE
	gbSTB &= ~STB_MSS;
	if ( (gbSTB ^ bLastSTB) & gbSTB & gbSRE )    // +ve transition
	{
		SET_BIT( gbSTB, STB_RQS );   // RQS = 1
		tmc_SRQpending = TRUE;
	}
	bLastSTB = gbSTB & ~STB_MSS;
#endif
}
/*_______________________________________________________________________________________*\
*
*                      DEVICE STATUS CHANGE & REPORTING FUNCTIONS
*
*
*   Device clear -- Resets the USBTMC comm's interface.
*/
void  device_clear( void )
{
	usbtmc_initialise();     // Clear the comm's interface
}
/*
*   Append a specified number of chars to the Command Message (bulk RX) buffer.
*/
void  usbtmc_command_cat( uchar *pbSource, short wCount )
{
	while ( wCount-- )
	{
		if ( iCmdMsgLen < MAX_USBTMC_CMD_LEN )
		{
			*pbCmdMsgBuf++ = (uchar) *pbSource++;
			iCmdMsgLen++ ;
		}
		else  yCmdBufOverflow = 1;
	}
}
/*
*   Append a specified number of chars to the Response Message (bulk TX) buffer.
*/
void  usbtmc_response_cat( u8 *pbSource, short wCount )
{
	u8 type;
	type=dmm_get_scpi_Interface_type();

if(type==0)
{
	while ( wCount-- )
	{
		if ( iRspMsgLen < MAX_USBTMC_RSP_LEN )
		{
			*pbRspMsgBuf++ = (uchar) *pbSource++;
			iRspMsgLen++ ;
		}
	}
}
else 
{
	 Scpi_write_data_usart(pbSource, wCount);
}

}
/*____________________________________________________________________________________________*\
*
*   USBTMC task scheduler --
*
*   Called *frequently* from the main kernel loop.
*/
extern u16 count_num;
void  usbtmc_scheduler( void )
{
	bool   yRespGend;
	// Check for Class Requests received on the Control Endpoint...
	/*	if ( tmc_DeviceClear  )         // USBTMC_INITIATE_CLEAR class request received
	{
	device_clear();
	tmc_DeviceClear = 0;
	} */
	if ( tmc_AbortBulkOutReq )
	{
		//	usbSetEndpointState( BULK_OUT_ENDP, ENDP_RESET );
		SetEPRxStatus(ENDP1, EP_RX_VALID  );
		yBulkOutTferInProgress = FALSE;
		yFirstPktRxTransfer = TRUE;        // Next pkt will be first of new transfer
		tmc_AbortBulkOutReq = FALSE;
	}
	if ( tmc_AbortBulkInReq )       // USBTMC_INITIATE_ABORT... class request received
	{
		SET_DEBUG_FLAG( 1 << 24 );
		usbtmc_task_abort_bulk_in();          // Send a null response
		wUSBTMCstate = USBTMC_Receiving_Cmd;
	}
	if ( expiredIndPulseTimer() )
	{
		//indicator_LED_toggle();
	}

	// IEEE488.2 Message Exchange Protocol state machine
	switch ( wUSBTMCstate )
	{
		case USBTMC_Receiving_Cmd:
			// Incoming <PGRM MESS> packets are appended to the command message buffer...
			if ( receive_stat )
			{	
				receive_stat=0;
				usbtmc_cmd_msg_handler();
			}
			// If a complete <PGRM MESSAGE> is available, call parser to process it...
			if ( yProgMessRecd )    // Got complete "program message"...
			{
				CLEAR_RESPONSE_MSG_BUFFER;
				if ( !yCmdBufOverflow )
				{
					if ( yDeviceMsgHdrRxd )
					yRespGend = parseDeviceCommand( abCommandBuf );
					else   // Assume VENDOR_CMD
					yRespGend = parseVendorCommand( abCommandBuf );
				}
				else  // <PGRM MESS> buffer overflow -- Reject the command!  */
				{
					SET_DEBUG_FLAG( 1 << 31 );
					signal_event( ESR_DDE );    // Signal 'Device-Dependent Error'
					yRespGend = FALSE;
					yCmdBufOverflow = 0;
				}
				if ( yRespGend )  /* The command generates a response... */
				{
					/* Assume a response msg has been written into the response buffer */
					//	SET_DEBUG_FLAG( 1 << 18 );
					//SET_BIT( gbSTB, STB_MAV );     /* Signal 'Message Available' */
					//device_status_update();
					tmc_DeviceClear=TRUE;
					//	wUSBTMCstate = USBTMC_Waiting_Req_Resp;
				}
				else
					tmc_DeviceClear=FALSE;
				
				yProgMessRecd = 0;
			}
			// Check for REQ_DEV_DEP_MSG_IN pkt rec'd without prior <PGRM MESS>...
			else if ( yRequestBulkInMsg )
			{
				//SET_DEBUG_FLAG( 1 << 28 );
				//	signal_event( ESR_QYE );    // Query Error!
				if(tmc_DeviceClear) 
				{
				wUSBTMCstate = USBTMC_Sending_Resp;
				tmc_DeviceClear=FALSE;
				}
				yRequestBulkInMsg = FALSE;
				//	yBulkInTferInProgress = 0;
			}

#if DT1_CAPABLE
			// Check for USBTMC_TRIGGER command pkt received
			else if ( yBusTriggerRecd  )
			{
				//			trigger_control();
				yBusTriggerRecd = FALSE;
			}
#endif
			break;  // case USBTMC_Receiving_Cmd

		case USBTMC_Waiting_Req_Resp:
			// Expecting command pkt of type 'USBTMC_REQUEST_DEV_DEP_MSG_IN'
			if (receive_stat)
			{	
				receive_stat=0;
				usbtmc_cmd_msg_handler();
			}
			if (yRequestBulkInMsg)
			{
				yRequestBulkInMsg = 0;
				wUSBTMCstate = USBTMC_Sending_Resp;
			}
			// Check if <PGRM MESSAGE> rec'd while expecting REQ_DEV_DEP_MSG_IN cmd.
			if ( yProgMessRecd )
			{
				SET_DEBUG_FLAG( 1 << 29 );
				signal_event( ESR_QYE );    // Query Error!
				CLEAR_COMMAND_MSG_BUFFER;   // Reject the command
				yProgMessRecd = 0;
				wUSBTMCstate = USBTMC_Receiving_Cmd;
			}
			break;

		case USBTMC_Sending_Resp:   // Transmitting a query response message

			//if ( yBulkInTferInProgress&&usbEndpointTxBuffReady(0))
			//	printf("USBTMC_Sending_Resp\r\n");
			// TxBusy=0;
			if ( yBulkInTferInProgress&&TxBusy)
			{
				// count_in=0;
				//	printf("yBulkInTferInProgress==1\r\n");
				usbtmc_rsp_msg_handler();   // Send (next) packet of response
				if ( yRespMsgSent )         // Done!
				{
				yRespMsgSent = 0;
				CLEAR_BIT( gbSTB, STB_MAV );     // Clear 'Message Available' flag
				device_status_update();
				wUSBTMCstate = USBTMC_Receiving_Cmd;
				}
			}
			break;
			
		default:
			usbtmc_initialise();
			break;

	}  // end switch
}
/*____________________________________________________________________________________________*\
*
*   USBTMC-USB488 COMMAND MESSAGE HANDLER
*
*   Called by the USBTMC scheduler routine whenever a Bulk Out (Rx) packet is received.
*   If the rec'd packet is the first packet of a transfer, the TMC Bulk Out header data is
*   extracted from the packet buffer and analysed. Message payload data is appended to the
*   command message buffer.
*
*   A complete message may consist of one or more transfers. The final (or single) transfer
*   is signalled by the EOM bit set in the 'TransferAttributes' byte of its header packet.
*   When a complete command message has been received, a flag is raised for the scheduler.
*
\*____________________________________________________________________________________________*/
u8 test_tmc;
void  usbtmc_cmd_msg_handler( void )
{
	static uint8  bTagPrevious = 0;    // Valid range is 1..255
	long   TransferSize;
	short  len;
	short  PktPayloadLen;    // Number of Msg data bytes in RX pkt
	uchar  MsgID = 0xFF;
	bool   MsgLenError = FALSE;
	extern u8 buffer_out[BULK_MAX_PKT_LEN];
	yBulkOutTferInProgress = TRUE;
	//len = usbReadEndpointData( BULK_OUT_ENDP, BulkOUTpkt, BULK_MAX_PKT_LEN );
	len=count_out;
	count_out=0;
	memcpy(BulkOUTpkt,buffer_out,BULK_MAX_PKT_LEN);
	if ( yFirstPktRxTransfer )
	{
		CLEAR_COMMAND_MSG_BUFFER;
		// Extract header info
		memcpy( &DevDepOutHeader, (const char *)BulkOUTpkt, USBTMC_MSG_HEADER_LEN );
		MsgID = DevDepOutHeader.MsgID;
		TransferSize = LITTLE_ENDIAN_TO_LONG( &DevDepOutHeader.TransferSize[0] );

#if FULL_USBTMC_BUILD
		// Check packet header;  if invalid, ignore the packet
		if ( len < USBTMC_MSG_HEADER_LEN
		|| DevDepOutHeader.bTagInverse != (~DevDepOutHeader.bTag & 0xFF) )
		{
			yBulkOutTferInProgress = FALSE;
			return;
		}
		// Check for packet sequence error
		if ( bTagPrevious != 0 && DevDepOutHeader.bTag == bTagPrevious )
		{
			yBulkOutTferInProgress = FALSE;
			return;
		}
		bTagPrevious = DevDepOutHeader.bTag;

#endif  // FULL_USBTMC_BUILD

		switch ( MsgID )
		{
			case USBTMC_DEV_DEP_MSG_OUT:
			case USBTMC_VENDOR_SPECIFIC_OUT:

				SET_DEBUG_FLAG( 1 << 16 );
				if ( MsgID == USBTMC_DEV_DEP_MSG_OUT ) 
					yDeviceMsgHdrRxd = 1;
				else  yDeviceMsgHdrRxd = 0;
					RxMsgBytesRemaining = TransferSize;
					
				PktPayloadLen = len - USBTMC_MSG_HEADER_LEN;
				if ( PktPayloadLen > TransferSize ) 
					PktPayloadLen = TransferSize;
				
				// Move message data from RX pkt struct to command buffer
				pBulkOUTpkt_data = BulkOUTpkt + USBTMC_MSG_HEADER_LEN;
				usbtmc_command_cat( pBulkOUTpkt_data, PktPayloadLen );
				RxMsgBytesRemaining -= PktPayloadLen;
				RxMsgBytesTransferred = PktPayloadLen;
				if ( MsgID == USBTMC_DEV_DEP_MSG_OUT )
				{
					yEndOfMessage = DevDepOutHeader.TransferAttributes & BIT_0;
					//	go_to_remote();     // Ref. USB488 spec. $4.3.2, Figure 2
				}
			break;

			case USBTMC_REQUEST_DEV_DEP_MSG_IN:
			case USBTMC_REQUEST_VENDOR_SPECIFIC_IN:

				SET_DEBUG_FLAG( 1 << 19 );
				yRequestBulkInMsg = TRUE;
				yBulkInTferInProgress = TRUE;     // USBTMC $3.3 Bulk-IN rules
				// Get info from incoming request pkt required for response pkt header
				memcpy( &ReqDevDepInHeader, (char *)BulkOUTpkt, USBTMC_MSG_HEADER_LEN );
				iMaxRespMsgLen = TransferSize;
				yFirstPktTxTransfer = TRUE;
				break;

#if DT1_CAPABLE
			case USBTMC_TRIGGER:

				yBusTriggerRecd = TRUE;
				go_to_remote();             // Ref. USB488 spec. $4.3.2, Figure 2
				break;
#endif
			default:    // Undefined MsgID

				//	usbSetEndpointState( BULK_OUT_ENDP, ENDP_STALL );    // Stall Bulk-OUT ep
				break;

		}  // end switch
	}
	else  // Not first packet of transfer -- all data is payload
	{
		if ( len < BULK_MAX_PKT_LEN )   // Short pkt => last pkt of tfr
		{
			if ( len < RxMsgBytesRemaining )    // Expecting more data than we got...
			{
				MsgLenError = TRUE;
				SET_DEBUG_FLAG( 1 << 30 );
			}
			// strip alignment bytes
			PktPayloadLen = LESSER_OF( RxMsgBytesRemaining, len );
		}
		else  // full packet -- may or may not be the last of this transfer
		{
			// In case this is the last pkt, strip alignment bytes
			PktPayloadLen = LESSER_OF( RxMsgBytesRemaining, BULK_MAX_PKT_LEN );
		}
		// Move message data from packet to command buffer
		usbtmc_command_cat( BulkOUTpkt, PktPayloadLen );
		RxMsgBytesRemaining -= PktPayloadLen;
		RxMsgBytesTransferred += PktPayloadLen;
	}

	// If receiving a message, check if the transfer is complete...
	if ( MsgID == USBTMC_DEV_DEP_MSG_OUT || MsgID == USBTMC_VENDOR_SPECIFIC_OUT )
	{
		if ( RxMsgBytesRemaining <= 0 )
		{
			yProgMessRecd = 1;
			RxMsgBytesRemaining = 0;
			yBulkOutTferInProgress = FALSE;
			yFirstPktRxTransfer = TRUE;        // Next packet will be first of new xfer
		}
		else  yFirstPktRxTransfer = FALSE;     // Expecting more packets in this xfer
	}
}

/*____________________________________________________________________________________________*\
*
*   USBTMC-USB488 RESPONSE MESSAGE HANDLER
*
*   The function assembles and transmits one (1) packet of buffered response data.
*   If the packet is the first packet of a transfer, the USBTMC Bulk In header data is
*   written into the packet. Payload data from the response buffer is then appended.
*   If the packet is not the first packet of a transfer, payload data is moved to the
*   packet from the response buffer, until all message bytes have been transferred.
*   The last pkt of a transfer must be either a short pkt or a zero-length pkt (ZLP).
*
*   Multiple-transfer responses are not supported by this implementation --
*   a complete response message must be buffered before this routine is called.
*
*   NB: In case of responses to USBTMC Vendor-Specific queries, the VISA host must set the
*   transfer size exactly to match the expected length of the reponse (because the EOM signal
*   is not supported by the standard Vendor-Specific protocol).
\*____________________________________________________________________________________________*/
void usbtmc_rsp_msg_handler( void )
{
	static bool   NeedNullPktLast;  // Set TRUE if we need to send a null pkt to end transfer
	static ulong  TransferSize;
	static short  PktPayloadLen;    // Pkt data payload len, including alignment bytes
	static short  PktMsgDataLen;    // Msg data bytes in pkt, excluding alignment bytes
	static short  PktTotalLen;
	uchar  sec_num=0;
	uchar  attributes = 0;
	bool   TransferComplete = 0;
	bool   ShortPacket = 0;;

	SET_DEBUG_FLAG( 1 << 17 );

	if ( yFirstPktTxTransfer )
	{
		// Prepare to send first packet of response
		SET_DEBUG_FLAG( 1 << 20 );
		memset( &DevDepInHeader, 0, USBTMC_MSG_HEADER_LEN );
		DevDepInHeader.MsgID = ReqDevDepInHeader.MsgID;
		// Copy the tag numbers from the last ReqDevDepInHeader packet to the reply header
		DevDepInHeader.bTag = ReqDevDepInHeader.bTag;
		DevDepInHeader.bTagInverse = ReqDevDepInHeader.bTagInverse;
		
		// Determine TransferSize and put this number in the packet header
		TxMsgBytesRemaining =  LESSER_OF( iRspMsgLen, iMaxRespMsgLen );
		TransferSize = TxMsgBytesRemaining;
		toLittleEndian( TransferSize, (uchar *) &DevDepInHeader.TransferSize[0] );
		if ( DevDepInHeader.MsgID == USBTMC_DEV_DEP_MSG_IN )
			attributes |= BIT_0;         // Set EOM bit -- we have a complete rsp message
		DevDepInHeader.TransferAttributes = attributes;
		PktMsgDataLen = (short) LESSER_OF( TxMsgBytesRemaining, FIRST_MAX_PAYLOAD_LEN );

		// Move header info to packet
		memcpy( (char *)BulkINpkt, &DevDepInHeader, USBTMC_MSG_HEADER_LEN );
		// Move "real" rsp msg data from Bulk Msg TX Buffer to pkt
		pBulkINpkt_data = BulkINpkt + USBTMC_MSG_HEADER_LEN;
		pbRspMsgBuf = (uchar *) abResponseBuf;
		memcpy( pBulkINpkt_data, pbRspMsgBuf, PktMsgDataLen );
		pbRspMsgBuf += PktMsgDataLen;
		// Pad Packet with alignment bytes to make PktPayloadLen a multiple of 4
		PktPayloadLen = PktMsgDataLen;
		pBulkINpkt_data += PktMsgDataLen;
		while ( PktPayloadLen % 4 )
		{
			*pBulkINpkt_data++ = 0;
			PktPayloadLen++ ;
		}
		if ( PktPayloadLen < FIRST_MAX_PAYLOAD_LEN )  ShortPacket = TRUE;
		PktTotalLen = PktPayloadLen + USBTMC_MSG_HEADER_LEN;

		// Actually transmit the packet
	//	usbTransmitEndpointData( BULK_IN_ENDP, BulkINpkt, PktTotalLen );
	 //  printf("BulkINpkt\r\n");
		USB_SEND(BulkINpkt,PktTotalLen);
		// If this is a full pkt, ensure a ZLP is sent last in case no more msg data
		if ( PktTotalLen == BULK_MAX_PKT_LEN ) NeedNullPktLast = TRUE;
		else  NeedNullPktLast = FALSE;
		TxMsgBytesTransferred = PktMsgDataLen;    // Not incl. header & align. bytes
		TxMsgBytesRemaining -= PktMsgDataLen;
		yFirstPktTxTransfer = FALSE;
		if ( ShortPacket )  TransferComplete = TRUE;
	}
	else   // Not first packet of transfer...
	{
	  if(TxBusy)
	  {
	//	 printf("SEC\r\n");
		// Prepare to send second or subsequent packet of response
		PktMsgDataLen = (short) LESSER_OF( TxMsgBytesRemaining, BULK_MAX_PKT_LEN );
		// Move "real" rsp msg data from Bulk Msg TX Buffer to pkt
		memcpy( BulkINpkt, pbRspMsgBuf, PktMsgDataLen );
		pbRspMsgBuf += PktMsgDataLen;
		// Pad Packet with alignment bytes to make PktPayloadLen a multiple of 4
		PktPayloadLen = PktMsgDataLen;
		pBulkINpkt_data = BulkINpkt + PktMsgDataLen;
		while ( PktPayloadLen % 4 )
		{
			*pBulkINpkt_data++ = 0;
			PktPayloadLen++ ;
		}	
		if ( PktPayloadLen < BULK_MAX_PKT_LEN )  ShortPacket = TRUE;

		if ( NeedNullPktLast && TxMsgBytesRemaining == 0 )    // ZLP to be sent now
		{
			PktMsgDataLen = 0;
			PktPayloadLen = 0;
			NeedNullPktLast = FALSE;
			ShortPacket = TRUE;
		}
		//sec_num=iRspMsgLen+USBTMC_MSG_HEADER_LEN-BULK_MAX_PKT_LEN ;
		// Actually transmit the packet
	//	usbTransmitEndpointData( BULK_IN_ENDP, BulkINpkt, PktPayloadLen );
	    USB_SEND(BulkINpkt,PktPayloadLen );
		//USB_SEND(BulkINpkt,sec_num);
		// If this is a full pkt, ensure a null pkt is sent last in case no more msg data
		if ( PktPayloadLen == BULK_MAX_PKT_LEN ) NeedNullPktLast = TRUE;
		else  NeedNullPktLast = FALSE;
		TxMsgBytesTransferred += PktMsgDataLen;
		TxMsgBytesRemaining -= PktMsgDataLen;
		if ( ShortPacket )  TransferComplete = TRUE;  	  
		if ( TxMsgBytesRemaining <= 0 && !NeedNullPktLast ) TransferComplete = TRUE; 
		}
	}

	if ( TransferComplete )   // Done!
	{
		SET_DEBUG_FLAG( 1 << 21 );
		yFirstPktTxTransfer = TRUE;
		yRespMsgSent = TRUE;
		TxMsgBytesRemaining = 0;
		yBulkInTferInProgress = FALSE;
		TxBusy=1;
	}
}
/*_______________________________________________________________________________________*\
*
*              USBTMC  COMMAND  PARSER/INTERPRETER  AND  COMMAND EXECUTION
*
*
*   The parser searches the command table for a match with the command string received.
*   If there's a match, it then finds and terminates (with a NUL) any command parameters
*   (which may be separated by a comma or spaces only). The command function is then
*   executed. If the command generates a response, the response string (formatted by the
*   command function) is terminated and the function returns TRUE; else it returns FALSE.
*/
extern  const commandTable  Command[];
bool  parseDeviceCommand( uchar abCmdBuf[] )
{
	char   *pCmdString;
	char    c, *pMsg;
	int     idx, idx2;
	u8    yRespGenerated = FALSE;
	bool    yFoundMatch = FALSE;
	bool    yEndOfMsg = FALSE;
	char    cTerm = '\n';
	int     len = 0;
   	u8 word_index = 0;
	u8 para_index = 0;
	pCmdString = (char *) abCmdBuf;
	while ( (c = *pCmdString) == SPACE || c == ':' )
	{
		pCmdString++ ;      // Skip leading white space and/or colon
	}
	pMsg = pCmdString;                  // Found start of first <PGRM MNEMONIC>
	while ( (c = *pMsg) != SPACE )      // Find length of first <PGRM MNEMONIC>
	{
		if ( c == ';' || c == ',' ) break;
		if ( !isprint( c ) ) break;         // End of <PGRM MNEMONIC>
		pMsg++ ;  len++ ;
	}
	for ( idx = 0 ; idx < MAX_TMC_COMMANDS ; idx++ )
	{
		if ( Command[idx].pMnemonic == NULL )   // Reached end of cmd table
			break;
		if ( strnmatch( pCmdString, Command[idx].pMnemonic, len ) )
	 // if(strcmp(pCmdString,sCommand[idx].pMnemonic))
		{
			yFoundMatch = TRUE;
			break;
		}
	}
	if ( !yFoundMatch ) 
	{ signal_event( ESR_CME );    // Error: Undefined command!
	  ERR_Num=3;
	  ERR_Flg=1;
	  beep_on=1;
	//  SHOW_WAMMING(0);
	}
	else
	{
		yRespGenerated = Command[idx].yRspGen;          // See if a response is pending.
		pCmdString = (char *) abCmdBuf;                  // Do parameter search...
		bParamCount = 0;
		for ( idx2 = 0 ; idx2 < MAX_CMD_PARAMS ; idx2++ )
		{
			if ( !isprint( *pCmdString ) ) break;        // end of message
			while ( *pCmdString == SPACE )               // skip leading spaces
			{
				pCmdString++ ;
			}
			if ( !isprint( *pCmdString ) ) break;        // end of message
			apParamStr[idx2] = pCmdString;               // Make ptr to param string
			bParamCount++ ;
			if(idx2==0)
			{
			while ( ( c = *pCmdString ) != SPACE )  
			{
				if ( !isprint( c ) )
				{ yEndOfMsg = TRUE;  break; }        // end of message
				pCmdString++ ;
			}
			}
			if(idx2>0)
			{
			while ( ( c = *pCmdString ) != SCPI_COMMA )       // find end of this param
			{
				if ( c == ',' ) break;                   // found separator (comma)
				if ( !isprint( c ) )
					{ yEndOfMsg = TRUE;  break; }        // end of message
				pCmdString++ ;
			}
			}
			*pCmdString++ = '\0';                        // terminate the param string
			if ( bParamCount == MAX_CMD_PARAMS ) break;  // Too many params
			if ( yEndOfMsg ) break;                      // end of message
		}
		bParamCount--;                                   // Don't count the cmd mnemonic
		if ( bParamCount == Command[idx].bParamsExpd )  // Found expected number of params
		{
			(*Command[idx].pFunction)();                // Execute the command function
			if ( yRespGenerated )
				usbtmc_response_cat( &cTerm, 1 );        // Terminate response msg
		}
		else if( bParamCount ==0&&(idx>=0&&idx<12))
		{
		 	Scpi_Measure_Qury=1;
			(*Command[idx].pFunction)();                // Execute the command function
			if ( yRespGenerated )
			usbtmc_response_cat( &cTerm, 1 );        // Terminate response msg
		}
		else  
		{
		ERR_Num=7;
		ERR_Flg=1;
		beep_on=1;
		signal_event( ESR_CME );
		}                   // Error: Wrong number of params!
	}
	return  yRespGenerated;
}

/*
*   Parser routine for USBTMC Vendor-Specific command/query messages.
*   If the command generates a response, the function returns TRUE.
*   *** Not supported in this example application ***
*/
bool  parseVendorCommand( uchar abCmdBuf[] )
{
	return  FALSE;
}

/*
*   Function:   strnmatch
*
*   Purpose:    Compares two ASCII strings.
*               Returns TRUE if the strings have identical content up to the specified
*               number of chars (iLength), even if one or both strings are longer.
*               Returns FALSE if either string is shorter than the specified length.
*               The comparison is NOT case-sensitive.
*               Valid terminators are NUL (0x00), NEWLINE (0x0A) and SPACE (0x20).
*
*   Args:       (char *) str1 :  pointer to string #1
*               (char *) str2 :  pointer to string #2
*               (short)  iLength :  maximum number of chars to compare.
*
*   Returns:    (bool) TRUE if strings match (not case-sensitive), else FALSE.
*/
u8  strnmatch( char *str1, char *str2, short iLength )
{
	char    c1, c2;
	short   wCount;
	u8   result = 0;
	U8  cnt,cnt1;
	cnt=strlen(str2) ;
	if(cnt!=iLength)
	return 0;
	for ( wCount = 0 ; wCount < iLength ; wCount++ )
	{
		c1 = toupper( *str1 );  str1++;
		c2 = toupper( *str2 );  str2++;
		if ( c1 == 0 || c1 == '\n' || c1 == ' ' ) break;  // end of string 1
		if ( c2 == 0 || c2 == '\n' || c2 == ' ' ) break;  // end of string 2
		if ( c1 != c2 ) break;    // mismatch
	}
	if ( wCount == cnt ) result = 1;
	return  result;
}
void  usbtmc_task_abort_bulk_in( void )
{
	uchar  attributes;

/*	DevDepInHeader.MsgID = USBTMC_DEV_DEP_MSG_IN;
	DevDepInHeader.bTag = ReqDevDepInHeader.bTag;
	DevDepInHeader.bTagInverse = ReqDevDepInHeader.bTagInverse;
	DevDepInHeader.reserved0 = 0;
	// Set transfer size = 0 in header
	toLittleEndian( (ulong) 0, (uchar *) &DevDepInHeader.TransferSize[0] );
	attributes = BIT_0;    // Set EOM bit
	DevDepInHeader.TransferAttributes = attributes;
	// Move header info to packet and send it out
	memcpy( (char *)BulkINpkt, &DevDepInHeader, USBTMC_MSG_HEADER_LEN );
	//usbTransmitEndpointData( BULK_IN_ENDP, BulkINpkt, USBTMC_MSG_HEADER_LEN );
	USB_CONTROL_SEND(BulkINpkt,USBTMC_MSG_HEADER_LEN); */
	// TODO .. BulkMsgTxBufferClear();
	yBulkInTferInProgress = FALSE;
	yFirstPktTxTransfer = TRUE;
	yRespMsgSent = FALSE;
	yCmdBufOverflow = FALSE;
	tmc_AbortBulkInReq = FALSE;
	wUSBTMCstate = USBTMC_Receiving_Cmd;
}
/*
*   If device_clear() is a scheduled task (delayed execution), then device_clear_done()
*   should return FALSE until the device_clear() task has completed, then return TRUE;
*/
bool  device_clear_done( void )
{
	return ( tmc_DeviceClear == 0 );
}
/*
*   Return the USBTMC_status, and the current Control-In buffer status.
*   When the function layer has completed the Device Clear sequence,
*   device_clear_done() will return TRUE.
*   Assume the current FIFO status is "clear".
*/
void UsbTMCCheckClearStatus( void )
{
	

	answer[0] = USBTMC_STATUS_SUCCESS;
	answer[1] = 0;

#if FULL_USBTMC_BUILD
	if ( !device_clear_done() ) answer[0] = USBTMC_STATUS_PENDING;
#endif

	//control_transmit( (uchar *) response, 2 );

//	DBG_PUTS("TMC Check Clr.\n");
}

/*
*   Return TRUE if specified IN (TX) endpoint FIFO is ready to accept data.
*   (The FIFO is ready for data to be written if yTxBusy[bEndp] == 0)
*   The function checks if any previous packet transmission is complete, and if so,
*   clears the yTxBusy[ep] signal.
*/
bool  usbEndpointTxBuffReady( uint8 bEndp )
{
	if ( abCommandBuf[0] == '\0'  )    // Last TX complete
	{
	 return 1;
	}
  else
	return 0;
}
/*
*   Abort Bulk-In (query response) message Transfer.
*   Host may continue to request Bulk In pkts until none remains, in order to flush
*   any unsent pkts already queued in the Bulk-IN FIFO buffer.
*   The device must not flush the Bulk-IN FIFO if a transfer is in progress.
*   The device must stop queueing any further response data until requested.
*   Return the USBTMC_status, and the most recent Bulk-IN bTag sent or received.
*   (Ref: USBTMC spec. Tables 24, 25, 26)
*/
void UsbTMCInitiateAbortBulkIn( void )
{

	uchar  bTag = LO_BYTE( ControlData.DeviceRequest.wValue );

	SET_DEBUG_FLAG( BIT_8 );

#if FULL_USBTMC_BUILD

	if ( yBulkInTferInProgress && bTag == ReqDevDepInHeader.bTag )
	{
		answer[0] = USBTMC_STATUS_SUCCESS;
		tmc_AbortBulkInReq = TRUE;      // Send a null response
	}
	else if ( yBulkInTferInProgress && bTag != ReqDevDepInHeader.bTag )
	{
		answer[0] = USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS;   // bTag mismatch
	}
	else if ( !yBulkInTferInProgress && !TxBusy  )
	{
		// No transfer in progress and Bulk-In FIFO not empty
	//	usbSetEndpointState( BULK_IN_ENDP, ENDP_RESET );
		SetEPTxStatus(ENDP2, EP_TX_VALID  ); 
		answer[0] = USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
	}
	else   // None of the above... ( yBulkInTferInProgress == FALSE )
	{
		answer[0] = USBTMC_STATUS_FAILED;
	}

#else
	answer[0] = USBTMC_STATUS_SUCCESS;
#endif

	answer[1] = ReqDevDepInHeader.bTag;
//	answer[1] =0;
//	control_transmit( (uchar *) response, 2 );
  // USB_CONTROL_SEND((uchar *) response,2);
//	DBG_PUTS("TMC Abort In.\n");
}
/*____________________________________________________________________________________________*\
*
*   Abort Bulk-Out command message Transfer.
*	Return the USBTMC_status, and the most recent Bulk-OUT bTag received.
*   Host will not send any further Bulk Out pkts in current (aborted) transfer.
*/
void UsbTMCInitiateAbortBulkOut( void )
{
	
	uchar  bTag = LO_BYTE( ControlData.DeviceRequest.wValue );

#if FULL_USBTMC_BUILD

	if ( yBulkOutTferInProgress && bTag == DevDepOutHeader.bTag )
	{
		answer[0] = USBTMC_STATUS_SUCCESS;
		//usbSetEndpointState( BULK_OUT_ENDP, ENDP_STALL );
		SetEPRxStatus(ENDP1, EP_RX_STALL);
		tmc_AbortBulkOutReq = TRUE;
	}
	else if ( yBulkOutTferInProgress && bTag != DevDepOutHeader.bTag )
	{
		// Abort packet ID (bTag) does not match the Bulk-Out transfer in progress
		answer[0] = USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
	}
	else if ( !yBulkOutTferInProgress&&receive_stat==1 )
	{
		receive_stat=0;
		// No transfer in progress and Bulk-Out FIFO is not empty
		answer[0] = USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
	}
	else   // None of the above
	{
		answer[0] = USBTMC_STATUS_FAILED;
	}

#else
	answer[0] = USBTMC_STATUS_SUCCESS;
#endif

	answer[1] = DevDepOutHeader.bTag;
	//control_transmit( (uchar *) response, 2 );   // Send EP0 response
	//USB_CONTROL_SEND((uchar *) response,2);
//	DBG_PUTS("TMC Abort Out.\n");
}
/*
*   USBTMC InitiateClear:  "...performs actions similar to those specified for Selected
*   Device Clear (IEEE488.1 bus SDC signal) in the IEEE 488 standard." (USB488 $4.2.1)
*
*   Setup a Halt condition on EP2 OUT, ie stall it.
*   Send status reply on Control-IN EP.
*   Flush main endpoint hardware TX and RX FIFO buffers.
*   Pass 'Device Clear' request to "function layer"... this resets the USBTMC comms
*   interface, clears the MEP (queues, etc), and restores IEEE/USB interrupts.
*/
void UsbTMCInitiateClear( void )
{
	uchar response[1];

	SET_DEBUG_FLAG( 1 << 27 );
//	usbSetEndpointState( BULK_OUT_ENDP, ENDP_STALL );
//	usbSetEndpointState( BULK_IN_ENDP, ENDP_RESET );
	SetEPRxStatus(ENDP1, EP_RX_STALL);
	SetEPTxStatus(ENDP2, EP_TX_NAK ); 
	answer[0] = USBTMC_STATUS_SUCCESS;
//	control_transmit( (uchar *) response, 1 );
//	USB_CONTROL_SEND((uchar *) response,1);
//#if FULL_USBTMC_BUILD
//	tmc_DeviceClear = TRUE;   // Signal to kernel to call device_clear() task
//#else
	device_clear();           // Just do it!
//#endif

//	DBG_PUTS("TMC Dev Clear.\n");
}
/*
*   USBTMC READ_STATUS_BYTE class request...
*   This function returns the IEEE488 Status Byte, and if SR1_CAPABLE, USBTMC_STATUS_XXXX.
*   Called from USB ISR (control dispatcher).
*
*   If the device is SR0 capable, ie. not SR1, the response containing STB is sent via the
*   Control-IN endpoint (EP0), with STB.bit6 == MSS (not RQS). (USB488 $4.3.1.1, Table 12).
*
*   If the device is SR1 capable, the Interrupt-IN Endpoint (EP1) must be configured for use,
*   and that endpoint must be used to return a response to a READ_STATUS_BYTE request.
*   In this case, the device responds in similar manner to the GPIB Serial Poll, i.e.
*   STB.bit6 = RQS flag.  RQS must be cleared after the STB is sent. (IEEE 488.2 $11.2.2.1)
*   A response must also be sent via the Control-IN Endpoint, but the packet content is
*   not the same as in the SR0 case (USB488 spec 3.4 & 4.3.1).
*/
void UsbTMC488ReadStatusByte( void )
{
	uchar  stb_bits = gbSTB;      // IEEE488.2 status byte
	uchar  response[3];
	uchar  bTag = ControlData.DeviceRequest.wValue & 0x7F;   // From the Setup pkt

	SET_DEBUG_FLAG( 1 << 26 );
	answer[0] = USBTMC_STATUS_SUCCESS;
	answer[1] = bTag;
	answer[2] = 0;

#if SR1_CAPABLE   // Respond on Interrupt-In AND Control-In endpoints

	if ( usbEndpointTxBuffReady( INTR_IN_ENDP ) )
	{
		// Interrupt-In TX FIFO is ready to accept data
		IntInResponsePkt.bNotify1 = bTag | BIT_7;
		IntInResponsePkt.bNotify2 = stb_bits;
		usbTransmitEndpointData( INTR_IN_ENDP, (uchar *) &IntInResponsePkt, 2 );
	}
	else  // Intrpt-In FIFO NOT ready for data -- do not queue Intrpt-In response
	{
		response[0] = USBTMC_STATUS_INTERRUPT_IN_BUSY;
		SET_DEBUG_FLAG( BIT_9 );
	}
	control_transmit( (uchar *) response, 3 );

#else    // Device is SR0 -- respond on Control-In ep only

	answer[2] = stb_bits;    /* bit6 = MSS */
	//control_transmit( (uchar *) response, 3 );
//	USB_CONTROL_SEND((uchar *) response,3);
#endif

//	DBG_PUTS("TMC Read STB.\n");
}
/*
*   Check progress of Bulk-Out abort sequence.
*	Return the USBTMC interface status, and the number of message data
*	bytes that have been received by the device so far in this transfer.
*/
void UsbTMCCheckAbortBulkOutStatus( void )
{
#if FULL_USBTMC_BUILD

//	uchar response[8];

	if ( tmc_AbortBulkOutReq )  answer[0] = USBTMC_STATUS_PENDING;
	else  answer[0] = USBTMC_STATUS_SUCCESS;
	answer[1] = 0x00;    // reserved
	answer[2] = 0x00;    // reserved
	answer[3] = 0x00;    // reserved
	toLittleEndian( RxMsgBytesTransferred, (uchar *) &answer[4] );
	//control_transmit( (uchar *) response, 8 );
	//USB_CONTROL_SEND((uchar *) response,8);
#else
	usbStallControlEndpoint();
#endif

	//DBG_PUTS("TMC Check Abt.\n");
}
/*
*   Check progress of Bulk-In abort sequence.
*   Return the USBTMC_status, and the number of message data bytes that have been transmitted
*   so far by the device in the current (aborted) transfer - TxMsgBytesTransferred.
*   Also indicates to host if the FIFO buffer has been cleared.
*   (Ref: USBTMC spec. Tables 27, 28, 29)
*/
void UsbTMCCheckAbortBulkInStatus( void )
{
#if FULL_USBTMC_BUILD

//	uchar response[8];

	if ( tmc_AbortBulkInReq )
	{
		answer[0] = USBTMC_STATUS_PENDING;
		answer[1] = 1;
		toLittleEndian( (long) 0, (uchar *) &answer[4] );
	}
	else
	{
		answer[0] = USBTMC_STATUS_SUCCESS;
		answer[1] = 0;
		toLittleEndian( TxMsgBytesTransferred, (uchar *) &answer[4] );
	}
	answer[2] = 0x00;    // reserved
	answer[3] = 0x00;    // reserved
//	control_transmit( (uchar *) response, 8 );
	//USB_CONTROL_SEND((uchar *) response,8);
#else
	usbStallControlEndpoint();
#endif

//	DBG_PUTS("TMC Check Abt.\n");
}

/*_______________________________________________________________________________________*\
*
*                             DEVICE  COMMAND  FUNCTIONS
*
*
*   Device function for common query:  *IDN?
*   Responds with the device IDN string.
*/
void  qry_idn( void )
{
	u8   cBuf[8];

	usbtmc_response_cat( zIDN_string, strlen(zIDN_string) );
	longToDecimalStr( BUILD_VER_MAJOR, cBuf, 1 );
	usbtmc_response_cat( cBuf, strlen(cBuf) );
	cBuf[0] = '.';
	usbtmc_response_cat( cBuf, 1 );
	longToDecimalStr( BUILD_VER_MINOR, cBuf, 1 );
	usbtmc_response_cat( cBuf, strlen(cBuf) );
	cBuf[0] = '.';
	usbtmc_response_cat( cBuf, 1 );
	longToDecimalStr( BUILD_VER_DEBUG, cBuf, 1 );
	usbtmc_response_cat( cBuf, strlen(cBuf) ); 
}
/*
*   Device function for common command:  *RST
*   Resets the device "function layer", but does not disconnect the USB interface.
*   The power-on/reset self-test procedure is executed.
*/

void  CalParam_Cls( void )
{
	cmd_count_reset();
	cmd_cls(); 
}
/*
*   Device function for common command:  *CLS
*   Clears the IEEE event status register (ESR) and updates STB accordingly.
*   In a real-world device application, *CLS clears ALL event status registers.
*/
void  cmd_cls( void )
{
	u8 i;
	u8 temp;
	ERR_Flg=0;
	temp=atoi( apParamStr[1] );
	if(temp>7)
	{
		for(i=0;i<12;i++)
		{
			PARA_K[i].a=1;
			PARA_K[i].b=0;
		}
	}
	else
	{
		PARA_K[temp].a=1;
		PARA_K[temp].b=0;
	}

}
/*
*   Device function for common query:  *STB?
*   Reponds with STB register value, formatted as 2 hexadecimal digits "#Hxx".
*   (The IEEE488.2 standard specifies a decimal value -- not as convenient as hex.)
*   STB bit assignments are defined in main.h.
*   Note: Bit_6 of the STB is the "Master Summary Status" bit, not the RQS flag,
*   when STB is read using this query.
*/
void  qry_stb( void )
{
	u8   cBuf[6];
	wordToHexStr( gbSTB, cBuf );
	cBuf[0] = '#';
	cBuf[1] = 'H';
	
	usbtmc_response_cat(cBuf , 2 );
		 
}

/*
*   Device function for common command:  *ESR
*   Responds with ESR register value, formatted as 2 hexadecimal digits "#Hxx",
*   leaving ESR cleared.
*   ESR bit assignments are defined in main.h.
*   (Although the ESR is 16 bits, only the 8 LS bits are defined by IEEE488.2)
*/
void  CalParam_Store( void )
{
	//PARA_K[8].a=0x55;
	//PARA_K[8].b=0x55;
	if(PARA_Saved(CAL_ADDRESS,&PARA_K[0], sizeof(PARA_K)))
	ERR_Flg=0;
	else
	{
	ERR_Num=2;
	ERR_Flg=1;
	beep_on=1;
	}
}
/*
*   Device function for common command:  *ESE <n> , or *ESE #Hxx
*   Sets the ESE register to the value given by the parameter, which may be a decimal
*   integer (0..255) or IEEE hex format "#Hxx", where 'xx' is a 2 digit hex number.
*   (Although the ESE is 16 bits, only the 8 LS bits are defined by IEEE488.2)
*/
void  cmd_ese( void )
{
	char   *pCmdParam = apParamStr[1];

	if ( *pCmdParam == '#' ) gwESE = hexatoi( pCmdParam+2 );
	else  gwESE = atoi( pCmdParam ) & 0xFF;
	device_status_update();	 
}
/*
*   Device function for common query:  *TST?
*   Responds with an error code indicating the outcome of the power-on/reset self-test.
*   If the device has passed all tests, the error code will be zero (0).
*   The response is formatted as 2 hexadecimal digits "#Hxx".
*   The self-test sequence is not initiated by this query -- it is executed at power-on/
*   processor reset only -- the error code is saved for the *TST? query.
*/
void  qry_tst( void )
{
	PARA_Read(CAL_ADDRESS,&PARA_K[0], sizeof(PARA_K));
}
/*
*   Device function for query:  COUNT:READ?
*   Responds with count value as decimal integer (ASCII numeric string)
*/
void  qry_count_read( void )
{
	char   cBuf[8];
	/*longToDecimalStr( (long) wButtonHitCounter, cBuf, 5 );
	usbtmc_response_cat( cBuf, strlen(cBuf) ); */
}
/*
*   Device function for command:  COUNT:RESET
*/
void  cmd_count_reset( void )
{
	#ifdef HIGHER_RESOLUTION
	u8 *cBuf="IDN-SGLT-PRI SPD3303S";
	#else
	u8 *cBuf="IDN-SGLT-PRI SPD3303D";
	#endif
	ERR_Flg=0;
	usbtmc_response_cat( cBuf, strlen(cBuf) );
}
/*
*   Device function for query:  INDICATOR?
*   The response string is either "ON" or "OFF".
*/
void  Upgrade_Rst( void )
{
	char   cBuf[4];

	cBuf[0] = ( yIndLEDstate ) ? 'O' : 'O';
	cBuf[1] = ( yIndLEDstate ) ? 'N' : 'F';
	cBuf[2] = ( yIndLEDstate ) ? ' ' : 'F';
	USB_DISABLE;
	//DELAY_MS(1);
	//	goto_RSTAddress();
	IWDG_Configuration();
	//	usbtmc_response_cat( cBuf, 3 );
}
/*
*   Device function for command:  INDICATOR <d>
*   ... where <d> is a single ASCII digit, i.e. '0' or '1'.
*   Controls the "READY" indicator LED -- ON (d != 0) or OFF (d == 0).
*   The 'USBTMC_INDICATOR_PULSE' class request controls the same LED.
*/
extern u8 key_stat;
extern u8 key_vale;
void  cmd_KEY( void )
{
	u8 *cBuf="SPD";
	ERR_Flg=0;
	usbtmc_response_cat( cBuf, strlen(cBuf) );
	/*int16  iwValue;

	iwValue = atoi( apParamStr[1] );
	key_vale=iwValue;
	key_stat=1;	*/
}
/*
*   Device function for query:  PARAM:ENQ?
*   The response string consists of 2 "device config. parameter" values, signed decimal
*   integers in the range -10000 to +10000, separated by a comma.
*/
void Serial_Fill(void)
{
	u8 i;
	u8 String_Serial[30]={0};
	String_Serial[0]=0x1e;
	String_Serial[1]=0x03;
	for(i=0;i<Serial_No_Long;i++)
	{
//		String_Serial[2+i*2]=para_factory.SERIAL_NO[i];
	}
	memcpy(Virtual_Com_Port_StringSerial,String_Serial,30);
}
void Serial_Default(void)
{
	u8 i;	  
	for(i=0;i<Serial_No_Long;i++)
	{
		para_factory.SERIAL_NO[i]=Virtual_Com_Port_StringSerial[2+i*2];
	}
	para_factory.SERIAL_NO[14]=0;
/*	if(set_store(para_store,Delault_Set_ADDRESS))
	{
	}	*/
}
void  Serial_Set( void )
{
	char   cBuf[8];
	ERR_Flg=0;
	if(strlen(apParamStr[1])<Serial_No_Long)
	{
		ERR_Num=2;
		ERR_Flg=1;
		beep_on=1;
		return;
	}
	//	memset(ptr_serial,0,strlen(ptr_serial));
	memset(para_factory.SERIAL_NO,0,23);
	//memcpy(ptr_serial,apParamStr[1],strlen(apParamStr[1]));
	memcpy(para_factory.SERIAL_NO,apParamStr[1],strlen(apParamStr[1]));
	para_factory.SERIAL_NO[14]=0;
	if(set_store(para_factory,Factory_Set_ADDRESS))
	{
	}
}

void Serial_Query( void )
{
	char   cBuf[8];
	ERR_Flg=0;
	usbtmc_response_cat( "SRLN Default,", strlen( "SRLN Default,") );
	usbtmc_response_cat( para_factory.SERIAL_NO, strlen(para_factory.SERIAL_NO) );
}
/*
*   Device function for command:  PARAM:SET <n1>, <n2>
*   The command string should include 2 "device config. parameter" values, signed decimal
*   integers in the range -10000 to +10000, separated by a comma (and optional space(s)).
*
*   In this example application, the "device config. parameters" are meaningless, but
*   serve to demonstrate a method of handling command parameters in a "program message".
*/
void  cmd_param_set( void )
{
	int16  iwValue;

	iwValue = atoi( apParamStr[1] );
	if ( iwValue >= -10000 && iwValue <= 10000 ) iwaConfigParam[0] = iwValue;
	else  signal_event( ESR_EXE );   // Error: Invalid param value!

	iwValue = atoi( apParamStr[2] );
	if ( iwValue >= -10000 && iwValue <= 10000 ) iwaConfigParam[1] = iwValue;
	else  signal_event( ESR_EXE );   // Error: Invalid param value!	
}
/*
*   Device function for query:  BUSY?
*   The response string is "YES" if the device is in the BUSY condition;  else "NO".
*/
void  qry_busy( void )
{
	char   cBuf[4];
	/*bool   busy = device_busy();

	cBuf[0] = ( busy ) ? 'Y' : 'N';
	cBuf[1] = ( busy ) ? 'E' : 'O';
	cBuf[2] = ( busy ) ? 'S' : ' ';
	usbtmc_response_cat( cBuf, 3 );	*/ 
}
/*
*   Device function for query: "DEBUG:FLAGS?"
*   Generates response string with USBTMC debug flags (8 hex digits)
*   in IEEE format (#Hxxxxxxxx) and then clears the flags.
*/
void  qry_debug_flags( void )
{
	char   cBuf[16];

/*	cBuf[0] = '#';
	cBuf[1] = 'H';
	wordToHexStr( (uint16)(glDebugFlags>>16), &cBuf[2] );   // MS word
	wordToHexStr( (uint16) glDebugFlags, &cBuf[6] );        // LS word
	usbtmc_response_cat( cBuf, 10 );
	glDebugFlags = 0; */
}
/*
*   Set nominated bit(s) in the Event Status Register (ESR) and update device status.
*   The argument, bmESR, is a bitmask with the required bit(s) set to 1.
*   (Bitmask symbols for standard ESR bits are defined in main.h)
*/
void  signal_event( uint8 bmESR )
{
	SET_BIT( gwESR, bmESR );
	device_status_update();
}
/*____________________________________________________________________________________________*\
*
*   Sundry USBTMC functions
\*____________________________________________________________________________________________*/

/*
*   Initialise the USBTMC-USB488 comm's interface --
*   Called by device_init() at power-on/reset and UsbTMCInitiateClear().
*   Must not be called before system interrupts enabled, i.e. before kernel is started.
*   If called from background, USB IRQs will be disabled on entry, restored on exit.
*   This function does not affect the Remote/Local state or REN.
*/
void usbtmc_initialise( void )
{
	CLEAR_COMMAND_MSG_BUFFER;
	CLEAR_RESPONSE_MSG_BUFFER;
	TxBusy=1;
	TxMsgBytesRemaining = 0;
	RxMsgBytesRemaining = 0;
	TxMsgBytesTransferred = 0;
	RxMsgBytesTransferred = 0;
	memset( &DevDepInHeader, 0, USBTMC_MSG_HEADER_LEN );
	memset( &ReqDevDepInHeader, 0, USBTMC_MSG_HEADER_LEN );
	memset( &abCommandBuf, 0, USBTMC_MSG_HEADER_LEN );
	memset( &abResponseBuf, 0, USBTMC_MSG_HEADER_LEN );
	yProgMessRecd = FALSE;
	yRespMsgSent = FALSE;
	yCmdBufOverflow = FALSE;
	yBusTriggerRecd = FALSE;

	tmc_DeviceClear = FALSE;
	tmc_SRQpending = FALSE;
	tmc_AbortBulkInReq = FALSE;
	tmc_AbortBulkOutReq = FALSE;
	yRequestBulkInMsg=FALSE;
	yBulkInTferInProgress = FALSE;
	yBulkOutTferInProgress = FALSE;
	yFirstPktRxTransfer = TRUE;
	yFirstPktTxTransfer = TRUE;
	yRespMsgSent = FALSE;

	wUSBTMCstate = USBTMC_Receiving_Cmd;
	//	Virtual_Com_Port_Reset();
	/*
	SetEPType(ENDP1, EP_BULK);
	SetEPRxAddr(ENDP1, ENDP1_RXADDR);
	SetEPRxCount(ENDP1, 64);
	SetEPRxStatus(ENDP1, EP_RX_VALID);
	SetEPTxStatus(ENDP1, EP_TX_DIS);

	SetEPType(ENDP2, EP_BULK);
	SetEPTxAddr(ENDP2, ENDP2_TXADDR);
	//SetEPTxCount(ENDP2, u16 wCount)
	SetEPTxStatus(ENDP2, EP_TX_NAK );
	SetEPRxStatus(ENDP2, EP_RX_DIS); */
	//SetEPTxValid(ENDP2);	
}

/*****
*   Convert unsigned long integer (32 bits) value to Little-endian,
*   stored as 4 byte array, LSB first.
*   This produces the same result with either big-endian or little-endian processor.
*
*   Entry args:     (uchar *) pDestin = pointer to LSB of Little-endian result
*                   (ulong)  value  = number to be converted
*   Returns:        --
*   Affects:        --
*/
void  toLittleEndian( uint32 value, uchar *pDestin )
{
	short   i;
	uchar  *pResult = pDestin;      // point to LSB of result

	for ( i = 0;  i < 4;  i++ )
	{
		*pResult++ = (uchar) (value & 0xFF);
		value = value >> 8;
	}
}
/*
*   Function:   longToDecimalStr
*
*   Purpose:    Converts a 32-bit longword to a signed decimal ASCII numeric string,
*               up to 10 digits & sign, right justified in the specified field width,
*               with leading zeros suppressed, and NUL-terminated.
*               If the value is too big to fit into the specified (minimum) field size,
*               the field will be expanded to accommodate the number of digits (& sign).
*               Negative numbers are formed with a 'minus' sign to the left of the first
*               non-zero digit. The field size includes the minus sign. Positive numbers
*               are output with space(s) to the left of the first digit, if any space(s)
*               can fit into the specified field width.
*
*   Args:       (int32) lVal       = longword to be converted
*               (char*) pcResult   = pointer to result string (which must be big enough!)
*               (uint8) bFieldSize = minimum number of character places in output (1..12)
*/
void  longToDecimalStr( int32 lVal, char *pcResult, uchar bFieldSize )
{
	uint8    acDigit[12];     // ASCII result string, acDigit[0] is LSD
	uint8    bPlace;
	uint8    c;
	uint8    bSignPlace = 0;
	bool     yNegative = FALSE;
	bool     yLeadingZero = TRUE;

	if ( bFieldSize > 12 )  bFieldSize = 12;
	if ( bFieldSize < 1 )  bFieldSize = 1;
	if ( lVal < 0 )  { yNegative = TRUE;  lVal = 0 - lVal; }   // make value absolute

	for ( bPlace = 0;  bPlace < 12;  bPlace++ )     // begin conversion with LSD
	{
		acDigit[bPlace] = '0' + (lVal % 10);
		lVal = lVal / 10;
	}

	for ( bPlace = 11;  bPlace < 12;  bPlace-- )    // begin processing with MSD
	{
		c = acDigit[bPlace];
		if ( bPlace != 0 && c == '0' && yLeadingZero )    // leave digit 0 (LSD) alone
			acDigit[bPlace] = SPACE;
		if ( bPlace == 0 || c != '0' )              // found 1st significant digit (MSD)
		{
			yLeadingZero = FALSE;
			if ( bSignPlace == 0 ) bSignPlace = bPlace + 1;
		}
	}
	if ( yNegative ) acDigit[bSignPlace] = '-';     // else there will be a SPACE already

	for ( bPlace = 11;  bPlace < 12;  bPlace-- )    // begin output with MSD (or SP, or sign)
	{
		c = acDigit[bPlace];
		if ( bPlace < bFieldSize || c != SPACE )
			*pcResult++ = c;
	}
	*pcResult = '\0';
}
/*
*   Convert integer value (4 LS bits) to hexadecimal ASCII character ('0' to 'F').
*   The input value is masked to use only the 4 LS bits.
*/
char  hexitoc( short wDigit )
{
	char  cRetVal;

	wDigit = wDigit & 0xF;
	if ( wDigit < 10 ) cRetVal = '0' + wDigit;
	else  cRetVal = 'A' + (wDigit - 10);

	return  cRetVal;
}

/*
*   Convert a 16-bit unsigned word to hexadecimal string (4 hex ASCII digits).
*   The result string is NOT terminated.
*/
void  wordToHexStr( uint16 wVal, char *pcResult )
{
	pcResult[0] = hexitoc( wVal >> 12 );   /* MSB first */
	pcResult[1] = hexitoc( wVal >> 8 );
	pcResult[2] = hexitoc( wVal >> 4 );
	pcResult[3] = hexitoc( wVal );
}

/*****
*  Convert Hexadecimal ASCII char (arg) to 4-bit value (returned as unsigned byte).
*
*  Called by:  various, background only
*  Entry args: c = Hex ASCII character
*  Returns:    0xFF if arg is not hex, else digit value as unsigned byte ( 0..0x0F )
*/

uint8  hexctobin( char c )
{
	if ( c >= '0'  &&  c <= '9')
		return ( c - '0' );
	else if ( c >= 'A'  &&  c <= 'F' )
		return ( c - 'A' + 10 );
	else if ( c >= 'a'  &&  c <= 'f' )
		return ( c - 'a' + 10 );
	else
		return 0xFF ;
}
/*****
*  Convert Hexadecimal ASCII string, up to 4 digits, to 16-bit unsigned word.
*  The string must be stored in the data RAM space.
*  There cannot be any leading white space.
*  Conversion is terminated when a non-Hex char is found.
*
*  Entry args: s = pointer to first char of hex string.
*  Returns:    Unsigned 16bit word ( 0 to 0xffff ).
*              If the target string (1st char) is non-Hex, returns 0.
*/

uint16  hexatoi( char * s )
{
	uint8   ubDigit, ubCount;
	uint16  uwResult = 0;

	for ( ubCount = 0;  ubCount < 4;  ubCount++ )
	{
		if ( (ubDigit = hexctobin( *s++ )) == 0xFF )
			break;
		uwResult = 16 * uwResult + ubDigit;
	}
	return  uwResult;
}
void INSTrument(void)
{
	if(strnmatch( apParamStr[1], CH1, 3 )	)
	{
		key_vale=KEY_CH1;
		key_stat=1;
	}
	else if(strnmatch( apParamStr[1], CH2, 3 )	)
	{
		key_vale=KEY_CH2;
		key_stat=1;
	}
	else if(strnmatch( apParamStr[1], CH3, 3 )	)
	{
		key_vale=KEY_CH3;
		key_stat=1;
	}

	else
	{
		ERR_Num=6;
		ERR_Flg=1;
		beep_on=1;
	}
}
void deal_scpi_usart_main(void)
{
	int len;
	if(USART_RX_STA&0x8000)
	{
			len=USART_RX_STA&0x3fff;//¦Ì?¦Ì?¡ä?¡ä??¨®¨º?¦Ì?¦Ì?¨ºy?Y3¡è?¨¨
			USART_RX_STA=0;
			//USB_scpi_input(USART_RX_BUF);
			parseDeviceCommand( USART_RX_BUF );
			memset( &USART_RX_BUF, 0, USART_REC_LEN );
	}
}
