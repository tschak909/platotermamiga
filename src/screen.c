/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * screen.h - Display output functions
 */

#include <stdlib.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <intuition/screens.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <graphics/gfxmacros.h>
#include <graphics/text.h>
#include <clib/diskfont_protos.h>
#include "screen.h"
#include "protocol.h"
#include "scale.h"
#include "font.h"

unsigned char CharWide=8;
unsigned char CharHigh=16;
unsigned char screen_mode;
extern padBool FastText; /* protocol.c */
padPt TTYLoc;
unsigned char FONT_SIZE_X=8;
unsigned char FONT_SIZE_Y=12;
unsigned short width;
unsigned short height;
unsigned long current_foreground=1;
unsigned long current_background=0;
padRGB current_foreground_rgb={255,255,255};
padRGB current_background_rgb={0,0,0};
ULONG* fontm23;
unsigned char is_mono=0;
unsigned char highest_color_index=0;
padRGB palette[16];

extern void done(void);

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct NewScreen Screen1 = {
  0,0,640,400,4,             /* Screen of 640 x 400 of depth 8 (2^4 = 16 colours)    */
  DETAILPEN, BLOCKPEN,
  HIRES|LACE,                     /* see graphics/view.h for view modes */
  PUBLICSCREEN,              /* Screen types */
  NULL,                      /* Text attributes (use defaults) */
  "PLATOTerm",
  NULL,
  NULL
};
struct Screen *myScreen;

struct Window *myWindow;
struct NewWindow winlayout = {
  0, 0,
  639, 399,
  0,1,
  IDCMP_CLOSEWINDOW|IDCMP_MENUPICK|IDCMP_ACTIVEWINDOW|IDCMP_VANILLAKEY|IDCMP_MOUSEMOVE|IDCMP_MENUPICK,
  WFLG_ACTIVATE|WFLG_BACKDROP|WFLG_BORDERLESS|WFLG_GIMMEZEROZERO,
  NULL, NULL,
  "PLATOTerm",
  NULL,NULL,
  640,400,
  640,400,
  CUSTOMSCREEN
};

struct TextAttr plato_ta = {"PLATO.font",12,0,0};
struct TextAttr platouser_ta = {"PLATOUser.font",12,0,0};
struct TextFont* platoFont;
struct TextFont* platoUserFont;

/* All of this is needed if we don't want area fills that are slower than
 * grandma on Nyquil. 
 * The TLDR; is the RastPort needs some scratch space for certain graphics
 * primitives to work well.
 */
struct TmpRas tmpras;
UBYTE *tmpbuf;
ULONG rassize;
struct AreaInfo areainfo;
UBYTE *areabuf;
#define MAXVEC 80

/**
 * screen_init() - Set up the screen
 */
void screen_init(void)
{
  unsigned char *bp;
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 34);

  if (!IntuitionBase)
    done();

  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",34);

  if (!GfxBase)
    done();

  myScreen = OpenScreen(&Screen1);
  if (!myScreen)
    done();
  winlayout.Screen=myScreen;

  myWindow = OpenWindow(&winlayout);

  if (!myWindow)
    done();
  /* Initilize scratch space for area fill primitives */
  rassize = RASSIZE(myWindow->GZZWidth,myWindow->GZZHeight);
  if(tmpbuf = AllocMem(rassize,MEMF_CHIP|MEMF_CLEAR))
  {
    InitTmpRas(&tmpras,tmpbuf,rassize);
    myWindow->RPort->TmpRas = &tmpras;
  }
  
  if(areabuf = AllocMem(8*MAXVEC,MEMF_CLEAR))
  {
      InitArea(&areainfo,areabuf,MAXVEC);
      myWindow->RPort->AreaInfo = &areainfo;
  }

  /* end of scratch space init XXX! add error checking */
  platoFont=OpenDiskFont(&plato_ta);

  if (!platoFont)
    done();

  platoUserFont=OpenDiskFont(&platouser_ta);

  if (!platoUserFont)
    done();

  // This is probably very wrong, but...hey...
  fontm23=platoUserFont->tf_CharData;
}

/**
 * screen_update_colors() - Set the terminal colors
 */
void screen_update_colors(void)
{
  long i=0;
  for (i=0;i<16;i++)
    {
      unsigned long r=palette[i].red   >> 4;
      unsigned long g=palette[i].green >> 4;
      unsigned long b=palette[i].blue  >> 4;
      SetRGB4(&myScreen->ViewPort,i,r,g,b);
    }
}

/**
 * screen_wait(void) - Sleep for approx 16.67ms
 */
void screen_wait(void)
{
}

/**
 * screen_beep(void) - Beep the terminal
 */
void screen_beep(void)
{
}

/**
 * screen_clear - Clear the screen
 */
void screen_clear(void)
{
  SetRast(myWindow->RPort,current_background);
  highest_color_index=0;
  palette[0].red=current_background_rgb.red;
  palette[0].green=current_background_rgb.green;
  palette[0].blue=current_background_rgb.blue;

  if ((current_background_rgb.red   != current_foreground_rgb.red) &&
      (current_background_rgb.green != current_foreground_rgb.green) &&
      (current_background_rgb.blue  != current_foreground_rgb.blue))
    {
      palette[1].red=current_foreground_rgb.red;
      palette[1].green=current_foreground_rgb.green;
      palette[1].blue=current_foreground_rgb.blue;
      highest_color_index++;
    }

  screen_update_colors();
}

/**
 * screen_set_pen_mode(void) - Set the pen mode
 */
void screen_set_pen_mode(void)
{
  if ((CurMode==ModeErase)||(CurMode==ModeInverse))
    SetAPen(myWindow->RPort,current_background);
  else
    SetAPen(myWindow->RPort,current_foreground);
}

/**
 * screen_block_draw(Coord1, Coord2) - Perform a block fill from Coord1 to Coord2
 */
void screen_block_draw(padPt* Coord1, padPt* Coord2)
{
  screen_set_pen_mode();
  RectFill(myWindow->RPort,
	   scalex[Coord1->x],
	   scaley[Coord1->y],
	   scalex[Coord2->x],
	   scaley[Coord2->y]);
}

/**
 * screen_dot_draw(Coord) - Plot a mode 0 pixel
 */
void screen_dot_draw(padPt* Coord)
{
  screen_set_pen_mode();
  WritePixel(myWindow->RPort,scalex[Coord->x],scaley[Coord->y]);
}

/**
 * screen_line_draw(Coord1, Coord2) - Draw a mode 1 line
 */
void screen_line_draw(padPt* Coord1, padPt* Coord2)
{
  screen_set_pen_mode();
  Move(myWindow->RPort,scalex[Coord1->x],scaley[Coord1->y]);
  Draw(myWindow->RPort,scalex[Coord2->x],scaley[Coord2->y]);
}

/**
 * screen_char_draw(Coord, ch, count) - Output buffer from ch* of length count as PLATO characters
 */
void screen_char_draw(padPt* Coord, unsigned char* ch, unsigned char count)
{
  struct Image textImage;
  unsigned short* curfont;
  short offset;
  unsigned short* choffset;
  unsigned char i=0;
  short x,y;
  
  switch(CurMem)
    {
    case M0:
      SetFont(myWindow->RPort,platoFont);
      offset=0;
      break;
    case M1:
      SetFont(myWindow->RPort,platoFont);
      offset=96;
      break;
    case M2:
      SetFont(myWindow->RPort,platoUserFont);
      // offset=0;
      offset=-32;
      break;
    case M3:
      SetFont(myWindow->RPort,platoUserFont);
      // offset=64;
      offset=32;
      break;
    }

  switch(CurMode)
    {
    case ModeWrite:
      SetDrMd(myWindow->RPort,JAM1);
      SetAPen(myWindow->RPort,current_foreground);
      break;
    case ModeRewrite:
      SetDrMd(myWindow->RPort,JAM2);
      SetAPen(myWindow->RPort,current_foreground);
      SetBPen(myWindow->RPort,current_background);
      break;
    case ModeErase:
      SetDrMd(myWindow->RPort,JAM1);
      SetAPen(myWindow->RPort,current_background);
      break;
    case ModeInverse:
      SetDrMd(myWindow->RPort,JAM2);
      SetAPen(myWindow->RPort,current_background);
      SetBPen(myWindow->RPort,current_foreground);
      break;
    }

  x=scalex[Coord->x];
  y=scaley[Coord->y]-4;

  for (i=0;i<count;++i)
    ch[i]+=offset;
  
  Move(myWindow->RPort,x,y);
  Text(myWindow->RPort,ch,count);

}

/**
 * screen_tty_char - Called to plot chars when in tty mode
 */
void screen_tty_char(padByte theChar)
{
  if ((theChar >= 0x20) && (theChar < 0x7F)) {
    screen_char_draw(&TTYLoc, &theChar, 1);
    TTYLoc.x += CharWide;
  }
  else if ((theChar == 0x0b)) /* Vertical Tab */
    {
      TTYLoc.y += CharHigh;
    }
  else if ((theChar == 0x08) && (TTYLoc.x > 7))	/* backspace */
    {
      TTYLoc.x -= CharWide;

      // Implement backspace.
    }
  else if (theChar == 0x0A)			/* line feed */
    TTYLoc.y -= CharHigh;
  else if (theChar == 0x0D)			/* carriage return */
    TTYLoc.x = 0;
  
  if (TTYLoc.x + CharWide > 511) {	/* wrap at right side */
    TTYLoc.x = 0;
    TTYLoc.y -= CharHigh;
  }
  
  if (TTYLoc.y < 0) {
    screen_clear();
    TTYLoc.y=495;
  }


}

/**
 * screen_dump_palette
 */
unsigned char screen_dump_palette(void)
{
  SetAPen(myWindow->RPort,current_foreground);
}

/**
 * screen_color_matching(color) - return index of matching color, or a new index, 
 * if not found.
 */
unsigned char screen_color_matching(padRGB* theColor)
{
  unsigned char i;
  for (i=0;i<16;i++)
    {
      if (i>highest_color_index)
	{
	  palette[i].red=theColor->red;
	  palette[i].green=theColor->green;
	  palette[i].blue=theColor->blue;
	  highest_color_index++;
	  return i;
	}
      else
	{
	  if ((palette[i].red==theColor->red) && 
	      (palette[i].green==theColor->green) && 
	      (palette[i].blue==theColor->blue))
	    {
	      return i;
	    }
	}
    }
}

/**
 * screen_foreground - set foreground color
 */
void screen_foreground(padRGB* theColor)
{
  current_foreground=screen_color_matching(theColor);
  screen_update_colors();
  screen_dump_palette();
}

/**
 * screen_background - set background_color
 */
void screen_background(padRGB* theColor)
{
  current_background=screen_color_matching(theColor);
  screen_update_colors();
  screen_dump_palette();
}

/**
 * screen_paint - Paint the screen
 * Calling convention is different due to this needing to be recursive.
 */
void screen_paint(padPt* Coord)
{
  SetOPen(myWindow->RPort,current_foreground);
  Flood(myWindow->RPort,0,scalex[Coord->x],scaley[Coord->y]);
  BNDRYOFF(myWindow->RPort);
}

/**
 * screen_done()
 * Close down TGI
 */
void screen_done(void)
{
  if (platoFont)
    CloseFont(platoFont);

  if (platoUserFont)
    CloseFont(platoUserFont);
  
  if (myWindow)
    CloseWindow(myWindow);
  if (myScreen)
    CloseScreen(myScreen);

  if (IntuitionBase)
    CloseLibrary((struct Library*)IntuitionBase);
  if (GfxBase)
    CloseLibrary((struct Library*)GfxBase);
}
