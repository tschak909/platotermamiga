/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * palette_debug.h - Palette Debug window
 */

#include <proto/intuition.h>
#include <intuition/screens.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <graphics/gfxmacros.h>
#include <graphics/text.h>
#include <clib/macros.h>
#include <clib/diskfont_protos.h>
#include <clib/alib_stdio_protos.h>
#include "palette_debug.h"

extern struct Window *myWindow;
extern struct Screen *myScreen;

/* remove these externs and place funcs into own file. */
extern void StripIntuiMessages(struct MsgPort* mp, struct Window* win );

unsigned char palette_debug_is_open=0; // Is palette_debug window open?

struct Window *windowPaletteDebug;

struct NewWindow windowPaletteDebugLayout = {
  0, 360,
  265, 32,
  0,1,
  0L, /* we don't want any IDCMP, we will set this up later */
  WFLG_ACTIVATE|WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_SMART_REFRESH,
  NULL, NULL,
  "PLATO Palette",
  NULL,NULL,
  16,16,
  265,32,
  CUSTOMSCREEN
};

/**
 * Show palette_debug window
 */
void palette_debug_show(void)
{
  if(palette_debug_is_open)
    return;
  windowPaletteDebugLayout.Screen = myScreen;
  windowPaletteDebug=OpenWindow(&windowPaletteDebugLayout);
  windowPaletteDebug->UserPort = myWindow->UserPort; /* Share our main UserPort */
  ModifyIDCMP(windowPaletteDebug,IDCMP_CLOSEWINDOW); /* Listen for CLOSEWINDOW */ 
  /* At this point we should still be getting IDCMP events for both windows on 
   * our main UserPor */
  palette_debug_is_open=1;  
}


/**
 * Palette_Debug done.
 */
void palette_debug_done(void)
{
  if (palette_debug_is_open==1)
  {
    /* We can't just call CloseWindow. it's not that simple. */
    /* The following is from the CloseWindow Autodocs */
    Forbid(); /* prevent race conditions with Intuition */
    /* Send back any messages for this window
     * that have not yet been processed 
     */
    StripIntuiMessages(windowPaletteDebug->UserPort, windowPaletteDebug);
    /* tell intuition that we don't want any more messages */
    windowPaletteDebug->UserPort = 0L;
    ModifyIDCMP(windowPaletteDebug,0L);
    /* turn multitasking back on */
    Permit();
    /* Finally close the window */
    CloseWindow(windowPaletteDebug);
    palette_debug_is_open = 0;
  }
}

/**
 * Update Palette Window
 */
void palette_debug_update(void)
{
  unsigned char i;

  if (palette_debug_is_open==0)
    return;
  
  for (i=0;i<8;i++)
    {
      SetAPen(windowPaletteDebug->RPort,i);
      SetOPen(windowPaletteDebug->RPort,15);
      RectFill(windowPaletteDebug->RPort,
	       i*32+4,12,
	       i*32+16+4,28);
    }
  BNDRYOFF(windowPaletteDebug->RPort);
}
