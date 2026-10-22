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
#include <intuition/screens.h>
#include <proto/graphics.h>
#include "clipboard.h"

#define HEADER_SIZE 12
#define CLIPBOARD_BUFFER_SIZE 2048 // 64x32
#define LF_PADDING_SIZE 32 // Size for additional LFs on each line.

extern void done(void);

unsigned char* clipboard_buffer;

struct MsgPort *clip_mp;
struct IOClipReq *clip_ior;

extern struct Screen* myScreen;
extern struct Window* myWindow;

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
  int line = clipboard_coord_y(Coord->y);
  int col = clipboard_coord_x(Coord->x);
  int written = 0;

  while (count > 0)
    {
      int chunk;

      /* The coordinates may be out of range, in which case the text
       * continues on the next line.
       */
      if (col >= 64)
        {
          col = 0;
          line++;
        }

      /* Don't run off the end of the clipboard buffer */
      if (line >= 32)
        break;

      /* Don't run past the end of the line */
      chunk = 64 - col;
      if (chunk > (int)count)
        chunk = count;

      memcpy(&clipboard_buffer[line*64+col],ch,chunk);

      ch += chunk;
      count -= chunk;
      written += chunk;
      col += chunk;

      if (col >= 64)
        {
          col = 0;
          line++;
        }
    }

  return written;
}

/**
 * Write 4 bytes to clipboard device. Used for headers
 */
void clipboard_write_long(ULONG* data)
{
  clip_ior->io_Data=(STRPTR)data;
  clip_ior->io_Length=4;
  clip_ior->io_Command=CMD_WRITE;
  DoIO((struct IORequest *)clip_ior);
}

/**
 * Write screen to clipboard
 */
void clipboard_write_screen(void)
{
  ULONG length;
  ULONG bmhdlength=20;
  ULONG cmaplength=24;
  ULONG camglength=4;
  ULONG dpiLength=4;
  ULONG dpiData=0;
  ULONG bodylength;
  unsigned short i,j;
  typedef UBYTE Masking;     /* Choice of masking technique. */

#define mskNone                0
#define mskHasMask             1
#define mskHasTransparentColor 2
#define mskLasso               3
  
  typedef UBYTE Compression;    /* Choice of compression algorithm
				   applied to the rows of all source and mask planes.  "cmpByteRun1"
				   is the byte run encoding described in Appendix C.  Do not compress
				   across rows! */
#define cmpNone        0
#define cmpByteRun1    1
  
  typedef struct _bmhdData {
    UWORD       w, h;             /* raster width & height in pixels      */
    WORD        x, y;             /* pixel position for this image        */
    UBYTE       nPlanes;          /* # source bitplanes                   */
    Masking     masking;
    Compression compression;
    UBYTE       pad1;             /* unused; ignore on read, write as 0   */
    UWORD       transparentColor; /* transparent "color number" (sort of) */
    UBYTE       xAspect, yAspect; /* pixel aspect, a ratio width : height */
    WORD        pageWidth, pageHeight; /* source "page" size in pixels    */
  } BMHDData;

  BMHDData bmhdData;

  /* The image is the raw contents of the window's bitmap, so its size
   * depends on the size of the bitmap, which is different in PAL mode.
   */
  bodylength=(ULONG)myWindow->RPort->BitMap->Rows*
             (ULONG)myWindow->RPort->BitMap->Depth*
             (ULONG)myWindow->RPort->BitMap->BytesPerRow;

  /* The FORM chunk contains the "ILBM" identifier and all of the chunks
   * that follow it.
   */
  length=4+28+32+12+12+8+bodylength;

  typedef struct {
    UBYTE red, green, blue;           /* color intensities 0..255 */
  } ColorRegister;                  /* size = 3 bytes           */
  
  typedef ColorRegister ColorMap[8];    /* size = 3n bytes          */

  ColorMap cmap;
  ULONG modeflags=0x8004;
  bmhdData.w=myWindow->RPort->BitMap->BytesPerRow*8;
  bmhdData.h=myWindow->RPort->BitMap->Rows;
  bmhdData.x=0;
  bmhdData.y=0;
  bmhdData.nPlanes=myWindow->RPort->BitMap->Depth;
  bmhdData.masking=0;
  bmhdData.compression=0;
  bmhdData.pad1=0;
  bmhdData.transparentColor=0;
  bmhdData.xAspect=10;
  bmhdData.yAspect=11;
  bmhdData.pageWidth=bmhdData.w;
  bmhdData.pageHeight=bmhdData.h;
  
  // Write header
  clip_ior->io_Offset=0;
  clip_ior->io_Error=0;
  clip_ior->io_ClipID=0;
  clipboard_write_long((ULONG *)"FORM");
  clipboard_write_long(&length);
  clipboard_write_long((ULONG *)"ILBM");


  // Write BMHD
  clipboard_write_long((ULONG *)"BMHD");
  clipboard_write_long(&bmhdlength);
  clip_ior->io_Data=(STRPTR)&bmhdData;
  clip_ior->io_Length=sizeof(bmhdData);
  clip_ior->io_Command=CMD_WRITE;
  DoIO((struct IORequest *)clip_ior);
  
  // Write CMAP
  clipboard_write_long((ULONG *)"CMAP");
  clipboard_write_long(&cmaplength);
  for (i=0;i<8;i++)
    {
      ULONG tmpColor;
      UBYTE r,g,b;
      tmpColor=GetRGB4(myScreen->ViewPort.ColorMap,i);
      cmap[i].red = (tmpColor >> 8) & 0x0F;
      cmap[i].red|= (cmap[i].red << 4);
      cmap[i].green = (tmpColor >> 4) & 0x0F;
      cmap[i].green|= (cmap[i].green<<4);
      cmap[i].blue = (tmpColor >> 0) & 0x0F;
      cmap[i].blue|=(cmap[i].blue<<4);
    }
  clip_ior->io_Data=(STRPTR)&cmap;
  clip_ior->io_Length=sizeof(ColorMap);
  clip_ior->io_Command=CMD_WRITE;
  DoIO((struct IORequest *)clip_ior);

  // Write CAMG
  clipboard_write_long((ULONG *)"CAMG");
  clipboard_write_long(&camglength);
  clipboard_write_long(&modeflags);

  clipboard_write_long((ULONG *)"DPI ");
  clipboard_write_long(&dpiLength);
  clipboard_write_long(&dpiData);
  
  // Write BODY
  clipboard_write_long((ULONG *)"BODY");
  clipboard_write_long(&bodylength);
  for (i=0;i<myWindow->RPort->BitMap->Rows;i++)
    {
      for (j=0;j<myWindow->RPort->BitMap->Depth;j++)
	{
	  clip_ior->io_Data=(PLANEPTR)&myWindow->RPort->BitMap->Planes[j][myWindow->RPort->BitMap->BytesPerRow*i];
	  clip_ior->io_Length=myWindow->RPort->BitMap->BytesPerRow;
	  clip_ior->io_Command=CMD_WRITE;
	  DoIO((struct IORequest *)clip_ior);
	}
    }

  // Inform clipboard we're done.
  clip_ior->io_Command=CMD_UPDATE;
  DoIO((struct IORequest *)clip_ior);
}

/**
 * Write text to clipboard
 */
void clipboard_write(void)
{
  ULONG length=HEADER_SIZE+CLIPBOARD_BUFFER_SIZE+LF_PADDING_SIZE;
  ULONG slength=CLIPBOARD_BUFFER_SIZE+LF_PADDING_SIZE;
  unsigned char line=0;
  
  // Write header
  clip_ior->io_Offset=0;
  clip_ior->io_Error=0;
  clip_ior->io_ClipID=0;
  clipboard_write_long((ULONG *)"FORM");
  clipboard_write_long(&length);
  clipboard_write_long((ULONG *)"FTXT");
  clipboard_write_long((ULONG *)"CHRS");
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
