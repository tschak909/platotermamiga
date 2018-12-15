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
#include <exec/memory.h>
#include <exec/ports.h>
#include <devices/serial.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "protocol.h"
#include "prefs.h"
#include "screen.h"
#include "menu.h"

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
 * io_init() - Set-up the I/O
 */
void io_init(void)
{

  ms = (MySer *) AllocMem(sizeof(MySer), MEMF_PUBLIC|MEMF_CLEAR);
  if (!ms)
    done();

 retry_io_open:  
  ms->readport=CreatePort(NULL,0L);
  ms->writeport=CreatePort(NULL,0L);
  if (!ms->readport || !ms->writeport)
    done();
  
  ms->readio  = (struct IOExtSer *)CreateExtIO(ms->readport,  sizeof(struct IOExtSer));
  ms->writeio = (struct IOExtSer *)CreateExtIO(ms->writeport, sizeof(struct IOExtSer));
  if (!ms->readio || !ms->writeio)
    done();
  
  if (OpenDevice(config.device_name,config.unit_number,(struct IORequest*)ms->readio,0L))
    {
      // We couldn't open the device named in preferences.
      if (strcmp(config.device_name,"serial.device")==0)
	done(); // we couldn't open the default serial.device, exit the program.
      else
	{
	  // serial device was changed from default, and it didn't open, display alert,
	  // reset to default, and try to open the default.
	  DisplayAlert(RECOVERY_ALERT,alertSerialDeviceError,52);
	  prefs_set_defaults();

	  // Clean up, and try again.
	  DeleteExtIO((struct IORequest *)ms->writeio);
	  DeleteExtIO((struct IORequest *)ms->readio);
	  DeletePort(ms->writeport);
	  DeletePort(ms->readport);
	  goto retry_io_open;
	}
    }

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
      // Abort any pending I/O transactions.
      if (ms->writeio != NULL)
	{
      /* We don't want to chance a hang on WaitIO so only
       * AbortIO if we have to 
       */
      if(!(CheckIO((struct IORequest *)ms->writeio)))
      {
          AbortIO((struct IORequest *)ms->writeio);
          WaitIO((struct IORequest *)ms->writeio);
      }
	}
      
      if (ms->readio != NULL)
	{
      /* We don't want to chance a hang on WaitIO so only
       * AbortIO if we have to 
       */
      if(!(CheckIO((struct IORequest *)ms->readio)))
      {
          AbortIO((struct IORequest *)ms->readio);
          WaitIO((struct IORequest *)ms->readio);
      }
	}

      // Close device
      if (device_is_opened==1)
	{
	  CloseDevice((struct IORequest *)ms->readio);
	  CloseDevice((struct IORequest *)ms->writeio);
	  device_is_opened=0;
	}

      // Delete the IO requests.
      if (ms->writeio != NULL)
	{
	  DeleteExtIO((struct IORequest *)ms->writeio);
	}
      if (ms->readio != NULL)
	{
	  DeleteExtIO((struct IORequest *)ms->readio);
	}

      // Delete the ports
      if (ms->writeport!=NULL)
	DeletePort(ms->writeport);
      if (ms->readport!=NULL)
	DeletePort(ms->readport);

      // Finally, free the serial I/O structure
      FreeMem(ms,sizeof(MySer));
    }
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
