/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * io.h - Input/output functions (serial/ethernet)
 */

#ifndef IO_H
#define IO_H

#define XON  0x11
#define XOFF 0x13

typedef struct _MySer
{
  UBYTE state; /* we track what is allocated or not in here */
  UBYTE error; /* error, check state */
  char name[40]; /*device name string */ 
  unsigned long unit_number; /* unit number for OpenDevice */
  struct MsgPort *writeport;  /** when the writeio is done, reply goes here **/
  struct MsgPort *readport;   /** ibid for the readio **/
  struct IOExtSer *readio, *writeio;
  UBYTE writedata[2048], readdata[2048];  /** data space **/
} MySer;

/* handle the open and close of a device using the MySer
 * struct to keep track of what needs to be done or undone.
 *
 * dev is a pointer to MySer and op will be one of the IO_...
 * operations defined in this header
 */
#define IO_STAT 0 /* get status, perhaps not needed */
#define IO_OPEN 1 /* open the device */
#define IO_CLOSE 2 /* close the device */
/* state bits */
#define WP_OPEN 0x01 /* ms->writeport is open */
#define RP_OPEN 0x02 /* ms->readport is open */
#define RIO_OPEN 0x04 /* ms->readio is open */
#define WIO_OPEN 0x08 /* ms->writeio is open */
#define DEV_OPEN 0x10 /* associated device is open */
/* return codes */
int io_ctl(MySer *dev,int op);
/**
 * io_init() - Set-up the I/O
 */
void io_init(void);

/**
 * io_send_byte(b) - Send specified byte out
 */
void io_send_byte(unsigned char b);

/**
 * io_status_baud(void) - Return current baud rate.
 */
int io_status_baud(void);

/**
 * io_status_rbuflen(void) - Return serial buffer length
 */
unsigned long io_status_rbuflen(void);

/**
 * io_status_flags(void) - Return serial flags
 */
char* io_status_serflags(void);

/**
 * io_main() - The IO main loop
 */
void io_main(void);

/**
 * io_done() - Called to close I/O
 */
void io_done(void);

/**
 * io_set_baud(baud_rate) - Set the serial device to given baud rate
 */
void io_set_baud(long baud_rate);

/**
 * io_toggle_rtscts(void) - Toggle RTS/CTS
 */
void io_toggle_rtscts(void);

#endif /* IO_H */
