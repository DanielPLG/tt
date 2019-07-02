/*
*   gendef.h
*
*   Common definitions for embedded C applications.
*/


#ifndef  _GENDEF_H_
#define  _GENDEF_H_


#ifndef TRUE
#define FALSE    0
#define TRUE     (!FALSE)
#endif

#ifndef NULL
#define NULL     ((void *) 0)
#endif

/*
*   PROCESSOR-INDEPENDENT DATA TYPES...
*   Some of these typedefs may need to be changed so that the data sizes implied by the
*   names of the generic types, int8, int16, etc, are correct for the target processor.
*   Use uint16 and uint32 in preference to ushort and ulong (resp) in new projects.
*/
typedef signed char         int8;
typedef unsigned char       uint8,  uchar;

typedef signed short        int16;
typedef unsigned short      uint16, ushort;

typedef signed long         int32;
typedef unsigned long       uint32, ulong;

#ifndef bool
typedef uchar       bool;
#endif

typedef void (* pfnvoid)(void);     /* pointer to void function */
/*************   Macros   ****************/

#define  until(exp)  while(!(exp))   /* Usage:  do { ... } until (exp);  */
#define  LITTLE_ENDIAN  TRUE    // SAM7 is set to little-endian mode (default)
#define  TEST_BIT(u, bm)   ((u) & (bm))
//#define  SET_BIT(u, bm)    ((u) |= (bm))
//#define  CLEAR_BIT(u, bm)  ((u) &= ~(bm))
#define ENDP0_RXADDR        (0x40)
#define ENDP0_TXADDR        (0x80)
#define ENDP1_RXADDR        (0xC0)
#define ENDP2_TXADDR        (0x110)
#define  SWAP(w)     ((((w) & 0xFF) << 8) | (((w) >> 8) & 0xFF))

#ifdef LITTLE_ENDIAN
#define  LSB_MSB(w)  (w)        // LSB is already first
#else
#define  LSB_MSB(w)  SWAP(w)    // Swap bytes to put LSB first
#endif

#define  HI_BYTE(w)  (((w) >> 8) & 0xFF)   /* Extract high-order byte from unsigned word */
#define  LO_BYTE(w)  ((w) & 0xFF)          /* Extract low-order byte from unsigned word */

#define  LESSER_OF(arg1,arg2)  ((arg1)<=(arg2)?(arg1):(arg2))
#define  FULL_USBTMC_BUILD   (1)    // Set False (0) for minimal USBTMC build

#define  ARRAY_SIZE(a)  (sizeof(a)/sizeof(a[0]))
#define  LITTLE_ENDIAN_TO_LONG(pul)  (*((unsigned long *)(pul)))
#define  SET_DEBUG_FLAG(bm)   (glDebugFlags |= bm)
/***** Commonly used symbollic constants *****/

#define  ERROR   (-1)
#define  FAIL    (-1)
#define  SUCCESS   0
#define  OFF       0
#define  ON        1
#define  DISABLE   0
#define  ENABLE    1

#define  BIT_0   0x01     /*** BIT MASKS ***/
#define  BIT_1   0x02
#define  BIT_2   0x04
#define  BIT_3   0x08
#define  BIT_4   0x10
#define  BIT_5   0x20
#define  BIT_6   0x40
#define  BIT_7   0x80
#define  BIT_8   0x0100
#define  BIT_9   0x0200
#define  BIT_10  0x0400
#define  BIT_11  0x0800
#define  BIT_12  0x1000
#define  BIT_13  0x2000
#define  BIT_14  0x4000
#define  BIT_15  0x8000

#define  NUL             0
#define  SPACE          32
#define SCPI_COMMA                   0x2C    // ,¶ººÅµÄASCII

#define  ASCII_NUL       0
#define  ASCII_ACK       6
#define  ASCII_BS        8        /* Ctrl+H, Backspace */
#define  ASCII_HT        9
#define  ASCII_TAB       9
#define  ASCII_LF       10
#define  ASCII_CR       13
#define  ASCII_DC2      18        /* Ctrl+R, Device Control 2 */
#define  ASCII_NAK      21
#define  ASCII_CAN      24        /* Ctrl+X, Cancel line */
#define  ASCII_ESC      27
#define  ASCII_SP       32
#endif  /* _GENDEF_H_ */
#define  BUILD_VER_MAJOR  1
#define  BUILD_VER_MINOR  0
#define  BUILD_VER_DEBUG  124
#define  BULK_MAX_PKT_LEN   64     /* Bulk-In/Out */
#define  INTR_MAX_PKT_LEN   64     /* Intrpt-In */
#define  MAX_TMC_COMMANDS   250     // Maximum number of TMC commands defined (set as req'd)
#define  MAX_CMD_PARAMS     10      // Maximum number of cmd params (incl. cmd menemonic)
 typedef struct commandTable_1
{
	char     *pMnemonic;      // Pointer to command definition (mnemonic string)
	u8      bParamsExpd;    // Number of parameters expected after cmd mnemonic
	pfnvoid   pFunction;      // Pointer to command function
	bool      yRspGen;        // True if command generates a response (i.e. is query)

}commandTable ;


