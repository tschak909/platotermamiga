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
#include <proto/utility.h>
#include <exec/memory.h>
#include <graphics/gfxmacros.h>
#include <graphics/text.h>
#include <clib/macros.h>
#include <clib/diskfont_protos.h>
#include <clib/alib_stdio_protos.h>
#include "io.h"
#include "sound.h"
#include "terminal.h"
#include "screen.h"
#include "protocol.h"
#include "scale.h"
#include "font.h"
#include "palette_debug.h"
#include "prefs.h"

#ifdef __VBCC__
#undef NULL
#define NULL 0L
#endif
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
unsigned char* fontm23;
unsigned short* fontm23_bold;
unsigned char is_mono=0;
unsigned char highest_color_index=0;
padRGB palette[16];

#define MAX(a,b)    ((a)>(b)?(a):(b))
#define MIN(a,b)    ((a)<(b)?(a):(b))

static char windowTitle[128]; /* Window title
                               * (used for overall status display) */

extern void done(void);

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct NewScreen Screen1 = {
  0,0,640,400,3,             /* Screen of 640 x 400 of depth 3 (2^3 = 8 colours)    */
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
  IDCMP_CLOSEWINDOW|IDCMP_MENUPICK|IDCMP_ACTIVEWINDOW|IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS|IDCMP_INTUITICKS|IDCMP_GADGETUP,
  WFLG_ACTIVATE|WFLG_BACKDROP|WFLG_BORDERLESS|WFLG_GIMMEZEROZERO,
  NULL, NULL,
  "PLATOTerm",
  NULL,NULL,
  640,400,
  640,400,
  CUSTOMSCREEN
};

/* All of this is needed if we don't want area fills that are slower than
 * grandma on Nyquil. 
 * The TLDR; is the RastPort needs some scratch space for certain graphics
 * primitives to work well.
 */
struct TmpRas tmpras;
PLANEPTR tmpbuf;
LONG rassize;
struct AreaInfo areainfo;
UWORD *areabuf;
BYTE lowmem_flag = 0; /* true if we are low on chipram when we started */
                      /* in future we can use other values than 1 
                       * for more aggressive memory saving options     */
#define MAXVEC 80     /* Maximum number of lines */
#define AREABUF_SIZE 5*MAXVEC /* 5 bytes per vertex */
/**
 * screen_init() - Set up the screen
 */
void screen_init(void)
{
  unsigned char *bp;
  ULONG chipram;
  /* before we start messing with the screen, lets check out much
   * chipram we have available. so we can do things like close the workbench */
  chipram = AvailMem(MEMF_CHIP|MEMF_LARGEST);
  lowmem_flag = chipram <= 319136;
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 34);

  if (!IntuitionBase)
    done();

  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",34);

  if (!GfxBase)
    done();

  if (lowmem_flag||config.close_workbench) /* low memory detcted close the workbench to help free memory */
      CloseWorkBench();

  myScreen = OpenScreen(&Screen1);
  if (!myScreen)
    done();
  winlayout.Screen=myScreen;

  myWindow = OpenWindow(&winlayout);

  if (!myWindow)
    done();
  /* Initilize scratch space for area fill primitives */
  if(config.paint_enabled) {
      rassize = RASSIZE(myWindow->GZZWidth,myWindow->GZZHeight);
      if(tmpbuf = AllocMem(rassize,MEMF_CHIP|MEMF_CLEAR))
      {
        InitTmpRas(&tmpras,tmpbuf,rassize);
        myWindow->RPort->TmpRas = &tmpras;
      }
  
      if(areabuf = AllocMem(AREABUF_SIZE,MEMF_CLEAR))
      {
          InitArea(&areainfo,areabuf,MAXVEC);
          myWindow->RPort->AreaInfo = &areainfo;
      }
  }

  /* end of scratch space init XXX! add error checking */
  font_init_platoFont();
  font_init_platoBoldFont();
  font_init_platoUserFont();
  font_init_platoBoldUserFont();
  AddFont(&platoFont);
  AddFont(&platoBoldFont);
  AddFont(&platoUserFont);
  AddFont(&platoBoldUserFont);
  
  // Reach into the font strike data...
  fontm23=(unsigned char*)platoUserFont.tf_CharData;
  fontm23_bold=(unsigned short*)platoBoldUserFont.tf_CharData;
}

/**
 * screen_update_colors() - Set the terminal colors
 */
void screen_update_colors(void)
{
  long i=0;
  for (i=0;i<8;i++)
    {
      unsigned long r=palette[i].red   >> 4;
      unsigned long g=palette[i].green >> 4;
      unsigned long b=palette[i].blue  >> 4;
      SetRGB4(&myScreen->ViewPort,i,r,g,b);
    }
}

/**
 * screen_update_title() - Update window/screen title
 */
void screen_update_title(void)
{
  sprintf(windowTitle,"PLATOTerm | %6ld | N81 | %s | %s | Recv Buf Len: %6ld",io_status_baud(),io_status_serflags(),terminal_status(),io_status_rbuflen());
  SetWindowTitles(myWindow,windowTitle,windowTitle);
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
    sound_beeper();
}

/**
 * screen_about(void) - Show about requester
 */
void screen_about(void)
{
  /* The "body" text for the requester: */
  
  struct IntuiText my_body_text2=
    {
      0,       /* FrontPen, colour 0 (blue). */
      0,       /* BackPen, not used since JAM1. */
      JAM1,    /* DrawMode, do not change the background. */
      15,      /* LedtEdge, 15 pixels out. */
      39,       /* TopEdge, 5 lines down. */
      NULL,    /* ITextFont, default font. */
      "    and William Schaub", /* IText, the body text. */
      NULL,
    };

  struct IntuiText my_body_text1=
    {
      0,       /* FrontPen, colour 0 (blue). */
      0,       /* BackPen, not used since JAM1. */
      JAM1,    /* DrawMode, do not change the background. */
      15,      /* LedtEdge, 15 pixels out. */
      27,       /* TopEdge, 5 lines down. */
      NULL,    /* ITextFont, default font. */
      "Written by: Thomas Cherryhomes", /* IText, the body text. */
      &my_body_text2,
    };

  struct IntuiText my_body_text=
    {
      0,       /* FrontPen, colour 0 (blue). */
      0,       /* BackPen, not used since JAM1. */
      JAM1,    /* DrawMode, do not change the background. */
      15,      /* LedtEdge, 15 pixels out. */
      5,       /* TopEdge, 5 lines down. */
      NULL,    /* ITextFont, default font. */
      "PLATOTerm for Amiga", /* IText, the body text. */
      &my_body_text1,
    };

  /* The negative text: */
  /* (Printed inside the right gadget) */
  struct IntuiText my_negative_text=
    {
      0,       /* FrontPen, colour 0 (blue). */
      0,       /* BackPen, not used since JAM1. */
      JAM1,    /* DrawMode, do not change the background. */
      6,       /* LedtEdge, 6 pixels out. */
      3,       /* TopEdge, 3 lines down. */
      NULL,    /* ITextFont, default font. */
      "OK",    /* IText, the text that will be printed. */
      NULL,    /* NextText, no more IntuiText structures link. */
    };

  char reqTitle[]="About PLATOTerm";
  SetWindowTitles(myWindow,reqTitle,reqTitle);
  AutoRequest(myWindow, &my_body_text, NULL, &my_negative_text, NULL, NULL, 320, 88);  
  screen_update_title();
}

/**
 * screen_clear_palette - Clear the palette
 */
void screen_clear_palette(void)
{
  int i;
  for (i=0;i<8;i++)
    {
      palette[i].red=0;
      palette[i].green=0;
      palette[i].blue=0;
    }

  /* Set complement color of 1 for HIGHCOMP */
  /* so menu texts are visible. */
  palette[7].red=255;
  palette[7].green=255;
  palette[7].blue=255;
}

/**
 * screen_clear - Clear the screen
 */
void screen_clear(void)
{
  highest_color_index=0;
  screen_clear_palette();

  current_foreground=current_background=0;
  
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
      current_foreground=1;
    }

  // Finally, a fall back, if somehow, color 0 and color 1 are black, fix it.
  if ((palette[1].red==0) &&
      (palette[1].green==0) &&
      (palette[1].blue==0) &&
      (palette[0].red==0) &&
      (palette[0].green==0) &&
      (palette[0].blue==0))
    {
      palette[0].red=palette[0].green=palette[0].blue=0;
      palette[1].red=palette[1].green=palette[1].blue=255;
      current_foreground=1;
      current_background=0;
      current_foreground_rgb.red=current_foreground_rgb.green=current_foreground_rgb.blue=255;
      current_background_rgb.red=current_foreground_rgb.green=current_foreground_rgb.blue=0;
    }
  
  screen_update_colors();
  SetRast(myWindow->RPort,current_background);
  SetAPen(myWindow->RPort,current_foreground);
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
	   MIN(scalex[Coord1->x],scalex[Coord2->x]),
	   MIN(scaley[Coord1->y],scaley[Coord2->y]),
	   MAX(scalex[Coord2->x],scalex[Coord1->x]),
	   MAX(scaley[Coord2->y],scaley[Coord1->y]));
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
      if (ModeBold)
	SetFont(myWindow->RPort,&platoBoldFont);
      else
	SetFont(myWindow->RPort,&platoFont);
      offset=0;
      break;
    case M1:
      if (ModeBold)
	SetFont(myWindow->RPort,&platoBoldFont);
      else
	SetFont(myWindow->RPort,&platoFont);
      offset=96;
      break;
    case M2:
      if (ModeBold)
	SetFont(myWindow->RPort,&platoBoldUserFont);
      else
	SetFont(myWindow->RPort,&platoUserFont);
      offset=-32;
      break;
    case M3:
      if (ModeBold)
	SetFont(myWindow->RPort,&platoBoldUserFont);
      else
	SetFont(myWindow->RPort,&platoUserFont);
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
  y=scaley[Coord->y]-3;

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
  padPt Coord1,Coord2;
  
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
      /* /\* SetAPen(myWindow->RPort,current_background); *\/ */
      /* TTYLoc.x -= CharWide; */
      /* RectFill(myWindow->RPort, */
      /* 	       MIN(scalex[TTYLoc.x],scalex[TTYLoc.x+CharWide]), */
      /* 	       MIN(scaley[TTYLoc.y],scaley[TTYLoc.y+CharHigh]), */
      /* 	       MAX(scalex[TTYLoc.x+CharWide],scalex[TTYLoc.x]), */
      /* 	       MAX(scaley[TTYLoc.y+CharHigh],scaley[TTYLoc.y])); */
      /* /\* SetAPen(myWindow->RPort,current_foreground); *\/ */
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
 * screen_color_matching(color) - return index of matching color, or a new index, 
 * if not found.
 */
unsigned char screen_color_matching(padRGB* theColor)
{
  unsigned char i;
  for (i=0;i<8;i++)
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
  current_foreground_rgb.red=theColor->red;
  current_foreground_rgb.green=theColor->green;
  current_foreground_rgb.blue=theColor->blue;
  current_foreground=screen_color_matching(theColor);
  screen_update_colors();
  SetAPen(myWindow->RPort,current_foreground);
}

/**
 * screen_background - set background_color
 */
void screen_background(padRGB* theColor)
{
  current_background_rgb.red=theColor->red;
  current_background_rgb.green=theColor->green;
  current_background_rgb.blue=theColor->blue;
  current_background=screen_color_matching(theColor);
  screen_update_colors();
  SetAPen(myWindow->RPort,current_foreground);
}

/**
 * screen_paint - Paint the screen
 * Calling convention is different due to this needing to be recursive.
 */
void screen_paint(padPt* Coord)
{
  if(!config.paint_enabled) return;
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
  RemFont(&platoFont);
  RemFont(&platoBoldFont);
  RemFont(&platoUserFont);
  RemFont(&platoBoldUserFont);
  
  if (myWindow)
    {
      /* deallocate tmpras and areainfo */
      if(config.paint_enabled) {
        myWindow->RPort->TmpRas = 0L;
        FreeMem(tmpbuf,(long)rassize);
        myWindow->RPort->AreaInfo = 0L;
        FreeMem(areabuf,(long)AREABUF_SIZE);
      }
      /* Now we can safely close the window */  
      CloseWindow(myWindow);
    }
  if (myScreen)
    CloseScreen(myScreen);
  
  if(lowmem_flag||config.close_workbench) /* we closed the workbench above, try to open it again */
      OpenWorkBench();

  if (IntuitionBase)
    CloseLibrary((struct Library*)IntuitionBase);
  if (GfxBase)
    CloseLibrary((struct Library*)GfxBase);
}
