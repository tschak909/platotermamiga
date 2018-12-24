/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * clipboard.h - Clipboard functions.
 */
#include <proto/exec.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <devices/clipboard.h>
#include <libraries/dosextens.h>
#include <libraries/dos.h>
#include <clib/alib_protos.h>
#include "clipboard.h"

#define HEADER_SIZE 12
#define CLIPBOARD_BUFFER_SIZE 2048 // 64x32
#define LF_PADDING_SIZE 32 // Size for additional LFs on each line.

extern void done(void);

unsigned char* clipboard_buffer;

struct MsgPort *clip_mp;
struct IOClipReq *clip_ior;

/**
 * Initialize the clipboard (set up text buffer)
 */
void clipboard_init(void)
{
  clipboard_buffer=AllocMem(CLIPBOARD_BUFFER_SIZE,MEMF_PUBLIC|MEMF_CLEAR);

  if (!clipboard_buffer)
    done();

  clipboard_clear();

  // Set up the clipboard device.
  clip_mp=CreatePort(0L,0L);

  if (!clip_mp)
    done();

  clip_ior = (struct IOClipReq *)CreateExtIO(clip_mp,sizeof(struct IOClipReq));

  if (!clip_ior)
    done();

  if (OpenDevice("clipboard.device",0,(struct IORequest *)clip_ior,0L))
    done();
  
}

/**
 * Clipboard Clear
 */
void clipboard_clear(void)
{
  unsigned char i;
  memset(clipboard_buffer,0x20,CLIPBOARD_BUFFER_SIZE);
}

/**
 * translate PLATO X coordinate (512) to nearest clipboard coordinate (64)
 */
unsigned char clipboard_coord_x(unsigned short x)
{
  return x>>3;
}

/**
 * translate PLATO Y coordinate (512) to nearest clipboard coordinate (32)
 */
unsigned char clipboard_coord_y(unsigned short y)
{
  return ((y^0x1FF)>>4);
}

/**
 * Write text to clipboard
 */
unsigned char clipboard_update_buffer(padPt* Coord, unsigned char* ch, unsigned char count)
{
  int dest = (clipboard_coord_y(Coord->y) * 64) + clipboard_coord_x(Coord->x);
  strncpy(&clipboard_buffer[dest],ch,count);
}

/**
 * Write 4 bytes to clipboard device. Used for headers
 */
void clipboard_write_long(long* data)
{
  clip_ior->io_Data=(STRPTR)data;
  clip_ior->io_Length=4;
  clip_ior->io_Command=CMD_WRITE;
  DoIO((struct IORequest *)clip_ior);
}

/**
 * Write text to clipboard
 */
void clipboard_write(void)
{
  unsigned long length=HEADER_SIZE+CLIPBOARD_BUFFER_SIZE+LF_PADDING_SIZE;
  unsigned long slength=CLIPBOARD_BUFFER_SIZE+LF_PADDING_SIZE;
  unsigned char line=0;
  
  // Write header
  clip_ior->io_Offset=0;
  clip_ior->io_Error=0;
  clip_ior->io_ClipID=0;
  clipboard_write_long((long *)"FORM");
  clipboard_write_long(&length);
  clipboard_write_long((long *)"FTXT");
  clipboard_write_long((long *)"CHRS");
  clipboard_write_long(&slength);

  for (line=0;line<32;line++)
    {
      // Write line
      clip_ior->io_Data=(STRPTR)&clipboard_buffer[line*64];
      clip_ior->io_Length=64;
      clip_ior->io_Command=CMD_WRITE;
      DoIO((struct IORequest *)clip_ior);

      // Write CR/LF.
      clip_ior->io_Data=(STRPTR)"\n";
      clip_ior->io_Length=1;
      clip_ior->io_Command=CMD_WRITE;
      DoIO((struct IORequest *)clip_ior);
    }

  // Inform clipboard we're done.
  clip_ior->io_Command=CMD_UPDATE;
  DoIO((struct IORequest *)clip_ior);
}

/**
 * Tear down the clipboard
 */
void clipboard_done(void)
{
  if (clipboard_buffer!=NULL)
    FreeMem(clipboard_buffer,(long)CLIPBOARD_BUFFER_SIZE);

  CloseDevice((struct IORequest *)clip_ior);
  DeleteExtIO((struct IORequest *)clip_ior);
  DeletePort(clip_mp);
  
}
