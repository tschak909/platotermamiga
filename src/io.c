/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * io.h - Input/output functions (serial/ethernet)
 */

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/ddebug_protos.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <devices/serial.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "protocol.h"
#include "prefs.h"
#include "screen.h"
#include "keyboard.h"
#include "menu.h"
#include "requester_devices.h"
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define true 1
#define false 0

unsigned char read_io_active=false;

extern void done(void);

MySer* ms;
struct Message* io_msg;

static unsigned char device_is_opened=0;

static UBYTE *alertSerialDeviceError =
  "\x00\x70\x14" "Could not open the selected serial device!" "\x00\x01"
  "\x00\x80\x24" "Press any Mouse Button to use defaults." "\x00";

static UBYTE *alertTrigger =
  "\x00\x70\x14" "Alert trigger!" "\x00";

/**
 * io_ctl() handle device open/close with state tracking
 * see io.h for details
 */
int io_ctl(MySer *dev, int op) {

    if(op == IO_OPEN) {
        if(dev->state != 0x0) { /* Open only makes sense with an empty state */
            dev->error = 1; 
            return 1;
        }
        dev->readport = CreatePort(NULL,0L);
        if(dev->readport) {
            dev->state |= RP_OPEN;
        }
        dev->writeport = CreatePort(NULL,0L);
        if(dev->writeport) {
            dev->state |= WP_OPEN;
        }
        dev->readio = (struct IOExtSer *)CreateExtIO(ms->readport,  sizeof(struct IOExtSer));
        if(dev->readio) {
            dev->state |= RIO_OPEN;
        }
        dev->writeio = (struct IOExtSer *)CreateExtIO(ms->readport,  sizeof(struct IOExtSer));
        if(dev->writeio) {
            dev->state |= WIO_OPEN;
        }

        if(dev->state == (RP_OPEN | WP_OPEN | RIO_OPEN | WIO_OPEN)) {
            if(OpenDevice(dev->name,dev->unit_number,(struct IORequest * )dev->readio,0L)){
                dev->error = 1;
                return 1;
            } else { 
                dev->state |= DEV_OPEN;
            }
        }
        /* Assert that everything was set open */
        if(dev->state == (RP_OPEN | WP_OPEN | RIO_OPEN | WIO_OPEN | DEV_OPEN)) {
            return 0;
        }
        /* we should not reach here */
        dev->error = 1;
        return 1;
    }
    
    /* Attempt to close each open resource in order */
    if(op == IO_CLOSE) {
        if(dev->state == 0x0) {
            return 0;
        }
        /* do flush IO/reset dance if it looks like everything is open */
        if(dev->state == (WP_OPEN|RP_OPEN|RIO_OPEN|WIO_OPEN|DEV_OPEN)) {
            dev->writeio->IOSer.io_Command = CMD_FLUSH;  /*flush all pending writes */
            DoIO((struct IORequest *)dev->writeio);
            if(!CheckIO((struct IORequest *)dev->writeio)) { /* check and abort them anyway if needed */
                AbortIO((struct IORequest *)dev->writeio);
                WaitIO((struct IORequest *)dev->writeio);
            }
            if(!CheckIO((struct IORequest *)dev->readio)) { /* same for reads */
                AbortIO((struct IORequest *)dev->readio);
                WaitIO((struct IORequest *)dev->readio);
            }
            dev->readio->IOSer.io_Command = CMD_RESET; /* reset the device just to be paranoid */
            dev->readio->IOSer.io_Flags = 0;
            SendIO((struct IORequest *)dev->readio);
        }
        /* otherwise we check the state in order and close whatever has it's bit set */
        if(dev->state & DEV_OPEN) {
            CloseDevice((struct IORequest *)dev->readio);
            dev->state &= ~(DEV_OPEN);
        }
        if(dev->state & WIO_OPEN) {
            DeleteExtIO((struct IORequest *)dev->writeio);
            dev->state &= ~(WIO_OPEN);
        }
        if(dev->state & RIO_OPEN) {
            DeleteExtIO((struct IORequest *)dev->readio);
            dev->state &= ~(RIO_OPEN);
        }
        if(dev->state & WP_OPEN) {
            DeletePort(dev->writeport);
            dev->state &= ~(WP_OPEN);
        }
        if(dev->state & RP_OPEN) {
            DeletePort(dev->readport);
            dev->state &= ~(RP_OPEN);
        }
        /* assert that state is now 0 */
        if(dev->state != 0x0) {
            dev->error = 1;
            return 1;
        }
        return 0; /*We've done everything we can and things should be a clean state of everything closed now */
    }
    return 2; /* we encountred an op we didn't recognise */
}
/**
 * io_init() - Set-up the I/O
 */
void io_init(void)
{

  ms = (MySer *) AllocMem(sizeof(MySer), MEMF_PUBLIC|MEMF_CLEAR);
  if (!ms)
    done();

 retry_io_open:  
  sprintf(ms->name,"%s",config.device_name);
  ms->unit_number = config.unit_number;
  if (io_ctl(ms,IO_OPEN))
    {
      // We couldn't open the device named in preferences.
      if (strcmp(ms->name,"serial.device")==0)
      {
          //can't open even serial.device so pop up the requester.
          extern struct Window *myWindow;
          extern unsigned char device_requester_is_active;
          io_ctl(ms,IO_CLOSE);
          requester_devices_run();
          for(;;) { /* this duplicates the mainloop but blocks io_init until the requester finishes */
              Wait(1L << myWindow->UserPort->mp_SigBit);   
              keyboard_main();
              if(device_requester_is_active == 0)
                  return;
          }
      }
      else
	{
	  // serial device was changed from default, and it didn't open, display alert,
	  // reset to default, and try to open the default.
	  DisplayAlert(RECOVERY_ALERT,alertSerialDeviceError,52);
	  prefs_set_defaults();

	  // Clean up, and try again.
      io_ctl(ms,IO_CLOSE);
	  goto retry_io_open;
	}
    }
  if(ms->error == 0)
      device_is_opened=1; // Device is now open.
  
  /* Set parameters from prefs */
  ms->readio->io_RBufLen=config.io_RBufLen;
  ms->readio->io_Baud=config.io_Baud;
  ms->readio->io_ReadLen=8;
  ms->readio->io_WriteLen=8;
  ms->readio->io_StopBits=1;
  ms->readio->io_SerFlags=SERF_XDISABLED|SERF_RAD_BOOGIE;

  if (config.rtscts_enabled==1)
    ms->readio->io_SerFlags|=SERF_7WIRE;
  
  ms->writeio->IOSer.io_Device=ms->readio->IOSer.io_Device;
  ms->writeio->IOSer.io_Unit = ms->readio->IOSer.io_Unit;
  ms->writeio->io_RBufLen=config.io_RBufLen;
  ms->writeio->io_Baud=config.io_Baud;
  ms->writeio->io_ReadLen=8;
  ms->writeio->io_WriteLen=8;
  ms->writeio->io_StopBits=1;
  ms->writeio->io_SerFlags=ms->readio->io_SerFlags;

  ms->readio->IOSer.io_Command = SDCMD_SETPARAMS;
  if (DoIO((struct IORequest *)ms->readio))
    done();

  menu_update_baud_rate(config.io_Baud);
  
  /* Serial port should be initialized, and open, at this point. */
  /* TBD: do baud rate setting */

  // Start the initial serial IO read.
  ms->readio->IOSer.io_Command = CMD_READ;
  ms->readio->IOSer.io_Flags = 0;
  ms->readio->IOSer.io_Length = 1;
  ms->readio->IOSer.io_Data = (APTR) ms->readdata;
  SendIO((struct IORequest *)ms->readio);
  read_io_active=true;
}

/**
 * io_create_ports(void)
 * Create I/O ports and requests
 */
void io_create_ports(void)
{
}

/**
 * io_send_byte(b) - Send specified byte out
 */
void io_send_byte(unsigned char b)
{
  if (read_io_active==false)
      return;
  if(device_is_opened == 0)
      return;
  ms->writeio->IOSer.io_Command = CMD_WRITE;
  ms->writeio->IOSer.io_Flags = 0;
  ms->writeio->IOSer.io_Length = 1;
  ms->writeio->IOSer.io_Data = (APTR) &b;
  DoIO((struct IORequest *)ms->writeio);  /** wait till serial device has sent it **/
}

/**
 * io_status_baud(void) - Return current baud rate.
 */
int io_status_baud(void)
{
  return ms->readio->io_Baud;
}

/**
 * io_status_flags(void) - Return serial flags
 */
char* io_status_serflags(void)
{
  if (ms->readio->io_SerFlags & SERF_7WIRE)
    {
      return "RTS/CTS ";
    }
  else if (ms->readio->io_SerFlags & (SERF_XDISABLED==0))
    {
      return "XON/XOFF";
    }
  else
    {
      return "NONE    ";
    }
}

/**
 * io_status_rbuflen(void) - Return serial buffer length
 */
unsigned long io_status_rbuflen(void)
{
  return ms->readio->io_RBufLen;
}

/**
 * io_main() - The IO main loop
 */
void io_main(void)
{
  long len;
  if (io_msg=GetMsg(ms->readport))
    {
      /* process pending single byte */
      ShowPLATO((padByte*)ms->readdata,1);

      /* Ask if there are any more bytes */
      ms->readio->IOSer.io_Command = SDCMD_QUERY;
      ms->readio->IOSer.io_Length = 0;
      DoIO((struct IORequest*)ms->readio);

      if (ms->readio->IOSer.io_Actual)
        {
	  /* Some bytes still in queue, grab and output. */
          ms->readio->IOSer.io_Command = CMD_READ;
          len = ms->readio->IOSer.io_Actual;
          len = MIN(256L, len);
          ms->readio->IOSer.io_Length = len;
          ms->readio->IOSer.io_Flags = 0;
          ms->readio->IOSer.io_Data = (APTR) ms->readdata;
          ms->readio->IOSer.io_Error = 0;
          DoIO((struct IORequest*)ms->readio);
	  ShowPLATO((padByte*)ms->readdata,len);
        }

      /* And finally, ask for another single byte. */
      ms->readio->IOSer.io_Command = CMD_READ;
      ms->readio->IOSer.io_Flags = 0;
      ms->readio->IOSer.io_Length = 1;
      ms->readio->IOSer.io_Data = (APTR) ms->readdata;
      SendIO((struct IORequest*)ms->readio);

    }
}

/**
 * io_done() - Called to close I/O
 */
void io_done(void)
{
  if (ms!=NULL)
    {
      io_ctl(ms,IO_CLOSE);
      // Finally, free the serial I/O structure
      FreeMem(ms,sizeof(MySer));
    }
   device_is_opened = 0;
   read_io_active = 0;
}

/**
 * io_set_baud(baud_rate) - Set the serial device to given baud rate
 */
void io_set_baud(long baud_rate)
{
  if (ms->readio==NULL)
    return;

  config.io_Baud=baud_rate;
  prefs_save();
  
  io_done();
  io_init();

  menu_update_baud_rate(baud_rate);
  screen_update_title();
}

/**
 * io_toggle_rtscts(void) - Toggle RTS/CTS
 */
void io_toggle_rtscts(void)
{
  config.rtscts_enabled^=1;
  prefs_save();

  io_done();
  io_init();

  screen_update_title();
}
