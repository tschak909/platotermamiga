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
#include <exec/memory.h>
#include <exec/ports.h>
#include <devices/serial.h>

#include "io.h"
#include "protocol.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define true 1
#define false 0

unsigned char read_io_active=false;

extern void done(void);

MySer* ms;
struct Message* io_msg;

/**
 * io_init() - Set-up the I/O
 */
void io_init(void)
{
  ms = (MySer *) AllocMem(sizeof(MySer), MEMF_PUBLIC|MEMF_CLEAR);
  if (!ms)
    done();

  ms->readport=CreatePort(NULL,0L);
  ms->writeport=CreatePort(NULL,0L);
  if (!ms->readport || !ms->writeport)
    done();

  ms->readio  = (struct IOExtSer *)CreateExtIO(ms->readport,  sizeof(struct IOExtSer));
  ms->writeio = (struct IOExtSer *)CreateExtIO(ms->writeport, sizeof(struct IOExtSer));
  if (!ms->readio || !ms->writeio)
    done();

  if (OpenDevice("serial.device",0,(struct IORequest*)ms->readio,0L))
    done();

  ms->writeio->IOSer.io_Device=ms->readio->IOSer.io_Device;
  ms->writeio->IOSer.io_Unit = ms->readio->IOSer.io_Unit;

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
  AbortIO((struct IORequest *)ms->writeio);
  WaitIO((struct IORequest *)ms->writeio);
  AbortIO((struct IORequest *)ms->readio);
  WaitIO((struct IORequest *)ms->readio);

  if (ms)
    {
      CloseDevice((struct IORequest *)ms->readio);
      CloseDevice((struct IORequest *)ms->writeio);
      DeleteExtIO((struct IORequest *)ms->writeio);
      DeleteExtIO((struct IORequest *)ms->readio);
      DeletePort(ms->writeport);
      DeletePort(ms->readport);
      FreeMem(ms,sizeof(MySer));
    }
}
