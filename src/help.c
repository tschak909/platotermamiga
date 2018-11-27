/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * help.c - Help windows
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
#include "help.h"

extern struct Window *myWindow;
extern struct Screen *myScreen;

unsigned char help_keys_is_open=0; // Is help window open?
extern struct IntuiMessage* intuition_msg;

static struct Window *windowHelpKeys;

/* The coordinates for the box: */
SHORT my_points[]=
{
   0,  0, /* Start at position (0,0) */
  70,  0, /* Draw a line to the right to position (70,0) */
  70, 10, /* Draw a line down to position (70,10) */
   0, 10, /* Draw a line to the right to position (0,10) */
   0,  0  /* Finish of by drawing a line up to position (0,0) */ 
};

/* The Border structure: */
struct Border my_border=
{
  0, 0,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  my_points,   /* XY, pointer to the array with the coordinates. */
  NULL,        /* NextBorder, no other Border structures are connected. */
};



/* The text string: */
UBYTE my_string[]="PRESS ME";

/* The IntuiText structure: */
struct IntuiText my_text=
{
  1,         /* FrontPen, colour register 1. */
  0,         /* BackPen, colour register 0. */
  JAM1,      /* DrawMode, draw the characters with colour 1, do not */
             /* change the background. */ 
  4, 2,      /* LeftEdge, TopEdge. */
  NULL,      /* ITextFont, use default font. */
  my_string, /* IText, the text that will be printed. */
             /* (Remember my_text = &my_text[0].) */
  NULL,      /* NextText, no other IntuiText structures are connected. */
};



struct Gadget my_gadget=
{
  NULL,          /* NextGadget, no more gadgets in the list. */
  40,            /* LeftEdge, 40 pixels out. */
  20,            /* TopEdge, 20 lines down. */
  71,            /* Width, 71 pixels wide. */
  11,            /* Height, 11 pixels lines heigh. */
  GADGHCOMP,     /* Flags, when this gadget is highlighted, the gadget */
                 /* will be rendered in the complement colours. */
                 /* (Colour 0 (00) will be changed to colour 3 (11) */
                 /* (Colour 1 (01)           - " -           2 (10) */
                 /* (Colour 2 (10)           - " -           1 (01) */
                 /* (Colour 3 (11)           - " -           0 (00) */  
  GADGIMMEDIATE| /* Activation, our program will recieve a message when */
  RELVERIFY,     /* the user has selected this gadget, and when the user */
                 /* has released it. */ 
  BOOLGADGET,    /* GadgetType, a Boolean gadget. */
  (APTR) &my_border, /* GadgetRender, a pointer to our Border structure. */
                 /* (Since Intuition does not know if this will be a */
                 /* pointer to a Border structure or an Image structure, */
                 /* Intuition expects an APTR (normal memory pointer). */
                 /* We will therefore have to calm down the compiler by */
                 /* doing some "casting".) */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. (We complement the */
                 /* colours instead) */
  &my_text,      /* GadgetText, a pointer to our IntuiText structure. */
                 /* (See chapter 3 GRAPHICS for more information) */
  NULL,          /* MutualExclude, no mutual exclude. */
  NULL,          /* SpecialInfo, NULL since this is a Boolean gadget. */
                 /* (It is not a Proportional/String or Integer gdget) */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};

struct NewWindow windowHelpKeysLayout = {
  479, 16,
  160, 383,
  0,1,
  IDCMP_CLOSEWINDOW,
  WFLG_ACTIVATE|WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_SIZEGADGET|WFLG_SIZEBRIGHT|WFLG_SIZEBBOTTOM,
  &my_gadget, NULL,
  "PLATO Keys",
  NULL,NULL,
  16,16,
  160,384,
  CUSTOMSCREEN
};



/**
 * Show key help
 * Assumed that screen is already initialized.
 */
void help_keys_show(void)
{
  windowHelpKeysLayout.Screen = myScreen;
  windowHelpKeys=OpenWindow(&windowHelpKeysLayout);
  windowHelpKeysLayout.FirstGadget = &my_gadget;
  help_keys_is_open=1;
}

/**
 * Key help main
 */
void help_keys_main(void)
{
  if (help_keys_is_open==0)
    return;

  while (intuition_msg = (struct IntuiMessage *) GetMsg(windowHelpKeys->UserPort))
    {
      if (intuition_msg->Class==IDCMP_CLOSEWINDOW)
	{
	  CloseWindow(windowHelpKeys);
	}
    }
    
}

/**
 * Key Help Close
 */
void help_done(void)
{
  if (help_keys_is_open==1)
    CloseWindow(windowHelpKeys);
}
