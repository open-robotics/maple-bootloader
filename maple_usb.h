#ifndef __MAPLE_USB_H
#define __MAPLE_USB_H

#include "maple_lib.h"
#include "maple_usb_desc.h"

/* USB configuration params */
#define BTABLE_ADDRESS  0x00
#define ENDP0_RXADDR    0x40
#define ENDP0_TXADDR    0x80    /* gives 64 bytes i/o buflen */
#define ENDP1_TXADDR    0xC0
#define ENDP2_TXADDR    0x100
#define ENDP3_RXADDR    0x110

#define COMM_ENB 1 /* enable the usb->serial */
#define VCOM_BUF_SIZE 0x40

#define VEND_ID0 0x10
#define VEND_ID1 0x01
#define PROD_ID0 0x01
#define PROD_ID1 0x10
#define PROD_ID0_DFU 0x01 /* in case we need to change the product ID after DFU enum */

#define bMaxPacketSize  0x40    /* 64B,  maximum for usb FS devices */
#define wTransferSize   0x0400  /* 1024B, want: maxpacket < wtransfer < 10KB (to ensure everything can live in ram */

#if COMM_ENB
#define NUM_ENDPTS      0x04
#else
#define NUM_ENDPTS      0x01
#endif

/* defines which interrupts are handled */
#define ISR_MSK (CNTR_CTRM   | \
                 CNTR_WKUPM  | \
                 CNTR_SUSPM  | \
                 CNTR_ERRM   | \
                 CNTR_SOFM   | \
                 CNTR_ESOFM  | \
                 CNTR_RESETM   \
		 )

/* command defines for virtual COM */
/* todo:  move to a maple_com src pair
   most of these commands arnt handled yet anyway
... */
#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE          0x02
#define GET_COMM_FEATURE          0x03
#define CLEAR_COMM_FEATURE        0x04
#define SET_LINE_CODING           0x20
#define GET_LINE_CODING           0x21
#define SET_CONTROL_LINE_STATE    0x22
#define SEND_BREAK                0x23

/* any structs or enums */
typedef struct _LINE_CODING
  {
    u32 bitrate;
    u8 format;
    u8 paritytype;
    u8 datatype;
  } LINE_CODING;

typedef enum _RESUME_STATE
  {
    RESUME_EXTERNAL,
    RESUME_INTERNAL,
    RESUME_LATER,
    RESUME_WAIT,
    RESUME_START,
    RESUME_ON,
    RESUME_OFF,
    RESUME_ESOF
  } RESUME_STATE;

typedef enum _DEVICE_STATE
  {
    UNCONNECTED,
    ATTACHED,
    POWERED,
    SUSPENDED,
    ADDRESSED,
    CONFIGURED
  } DEVICE_STATE;

/* a few externs from the virtual COM, eventually move to dedicated sources */
extern LINE_CODING linecoding;
extern u8 vcom_buffer_out[VCOM_BUF_SIZE];
extern u32 vcom_count_in;
extern u32 vcom_count_out;

/* public exported funcs */
void usbAppInit(void); /* singleton usb initializer */

/* internal usb HW layer power management */
void usbSuspend(void);
void usbResumeInit(void);
void usbResume(RESUME_STATE state);
RESULT usbPowerOn(void);
RESULT usbPowerOff(void);

/* internal functions (as per the usb_core pProperty structure) */
void usbInit(void);
void usbReset(void);
void usbStatusIn(void);
void usbStatusOut(void);
RESULT usbDataSetup(u8 request);
RESULT usbNoDataSetup(u8 request);
RESULT usbGetInterfaceSetting(u8,u8);
u8* usbGetDeviceDescriptor(u16 length);
u8* usbGetConfigDescriptor(u16 length);
u8* usbGetStringDescriptor(u16 length);
u8* usbGetFunctionalDescriptor(u16 length);

/* internal callbacks to respond to standard requests */
void usbGetConfiguration(void);
void usbSetConfiguration(void);
void usbGetInterface(void);
void usbSetInterface(void);
void usbGetStatus(void);
void usbClearFeature(void);
void usbSetEndpointFeature(void);
void usbSetDeviceFeature(void);
void usbSetDeviceAddress(void);

/* the small number of comm emulator functions to 
   eventually be migrated into their own usart sources
*/
u8* vcomGetLineCoding(u16 length);
u8* vcomSetLineCoding(u16 length);
void vcomEp1In(void);
void vcomEp3Out(void);

/* Interrupt setup/handling exposed only so that 
   its obvious from main what interrupts are overloaded 
   from c_only_startup.s (see the top of main.c) */
void usbDsbISR(void);
void usbEnbISR(void);
void usbISTR(void);

void nothingProc(void);

/*
notes from manual:
USB Base = 0x4005C00
USB_CNTR = USB+0x40, resets to 0x03. bit 0 is rw FRES (force reset, used in DFU_DETACH)
must manually set AND clear FRES
 */

#endif
