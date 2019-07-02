/*
*   main.h
*/

#ifndef  _MAIN_H_
#define  _MAIN_H_


// Bit definitions for the IEEE488 Standard 'Status Byte' (STB)...
// Bits 3..0 are device-specific event register summary bits (0 if undefined)
#define  STB_OSS    BIT_7    // Operation Status register Summary bit (not used)
#define  STB_MSS    BIT_6    // Master Status Summary bit
#define  STB_RQS    BIT_6    // Request for Service (Only if device is 'SR1')
#define  STB_ESB    BIT_5    // Event Status register (ESR) summary bit
#define  STB_MAV    BIT_4    // Message Available signal (Query response ready)

// Bit definitions for the IEEE488 Standard 'Event Status Register' (ESR)...
// and 'Event Status Enable' register (ESE)
// (Bits 15..8 are reserved for future IEEE488.2 standard revisions -- always 0)
#define  ESR_PON    BIT_7    // Power-ON (or hardware reset occurred)
#define  ESR_URQ    BIT_6    // User Request (Device-specific event)
#define  ESR_CME    BIT_5    // Command Error (bad syntax or semantics)
#define  ESR_EXE    BIT_4    // Execution Error (command rejected, e.g. bad param)
#define  ESR_DDE    BIT_3    // Device-Dependent Event/Error ^^
#define  ESR_QYE    BIT_2    // Query Error (No response pending when requested)
#define  ESR_RTL    BIT_1    // Returned to Local control (if RL1_CAPABLE)
#define  ESR_OPC    BIT_0    // Operation Complete  (always 0 - not supported)

// ^^ DDE could be a Summary Bit associated with a "Device Status Regiser" (DSR)
//    ... however, in this example, DDE signals a "message protocol error".


extern  bool    gb5msecTaskReq;
extern  bool    gb50mSecTaskReq;
extern  bool    gb500msecTaskReq;

extern  uint8   gbSTB;       // Status Byte register (STB)
extern  uint8   gbSRE;       // Service Request Enable (SRE) register
extern  uint16  gwESE;       // Event Status Enable (ESE) register
extern  uint16  gwESR;       // Event Status Register (ESR)

extern  uint32  glDebugFlags;
extern  uint16  wButtonHitCounter;

// Main functions --
void   taskDispatcher( void );
void   count_button_hits( void );
bool   button_hit( uint32 bmSwMask );
void   button_input_scan( void );
void   startIndPulseTimer( void );
bool   expiredIndPulseTimer( void );

// Command processing functions --
bool   parseDeviceCommand( uchar abCmdBuf[] );
bool   parseVendorCommand( uchar abCmdBuf[] );
bool   strnmatch( char *str1, char *str2, short wCount );

// Standard (IEEE488/SCPI) command/query functions --
void   qry_idn( void );
void   CalParam_Cls( void );
void   cmd_cls( void );
void   qry_stb( void );
void   CalParam_Store( void );
void   cmd_ese( void );
void   qry_tst( void );

// Device-specific command/query functions --
void   qry_count_read( void );
void   cmd_count_reset( void );
void   Upgrade_Rst( void );
void   cmd_indicator( void );
void   Serial_Set( void );
void   cmd_param_set( void );
void   qry_busy( void );
void   cmd_boot( void );
void   qry_debug_flags( void );
void  cmd_KEY( void );
// Device status handling functions --
void   device_clear( void );
bool   device_clear_done( void );
bool   device_busy( void );
void   device_status_update( void );
void   signal_event( uint8 bmESR );

#endif  // _MAIN_H_
