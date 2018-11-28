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
struct IntuiMessage* help_intuition_msg;

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


/**
 * IntuiText contents
 */
static struct IntuiText textKeys17 =
  {1,0,JAM1,0,204,NULL,"ERASE    BACKSP",NULL};
static struct IntuiText textKeys16 =
  {1,0,JAM1,0,192,NULL,"SUB      CTRL-Y",&textKeys17};
static struct IntuiText textKeys15 =
  {1,0,JAM1,0,180,NULL,"MULTIPLY CTRL-X",&textKeys16};
static struct IntuiText textKeys14 =
  {1,0,JAM1,0,168,NULL,"TERM     CTRL-T",&textKeys15};
static struct IntuiText textKeys13 =
  {1,0,JAM1,0,156,NULL,"STOP     CTRL-S",&textKeys14};
static struct IntuiText textKeys12 =
  {1,0,JAM1,0,144,NULL,"SQUARE   CTRl-Q",&textKeys13};
static struct IntuiText textKeys11 =
  {1,0,JAM1,0,132,NULL,"SUPER    CTRL-P",&textKeys12};
static struct IntuiText textKeys10 =
  {1,0,JAM1,0,120,NULL,"NEXT     ENTER",&textKeys11};
static struct IntuiText textKeys9 =
  {1,0,JAM1,0,108,NULL,"MICRO    CTRL-N",&textKeys10};
static struct IntuiText textKeys8 =
  {1,0,JAM1,0,96,NULL,"LAB      CTRL-L",&textKeys9};
static struct IntuiText textKeys7 =
  {1,0,JAM1,0,84,NULL,"HELP     HELP",&textKeys8};
static struct IntuiText textKeys6 =
  {1,0,JAM1,0,72,NULL,"DIVIDE   CTRL-G",&textKeys7};
static struct IntuiText textKeys5 =
  {1,0,JAM1,0,60,NULL,"FONT     CTRL-F",&textKeys6};
static struct IntuiText textKeys4 =
  {1,0,JAM1,0,48,NULL,"EDIT     CTRL-E",&textKeys5};
static struct IntuiText textKeys3 =
  {1,0,JAM1,0,36,NULL,"DATA     CTRL-D",&textKeys4};
static struct IntuiText textKeys2 =
  {1,0,JAM1,0,24,NULL,"COPY     CTRL-C",&textKeys3};
static struct IntuiText textKeys1 =
  {1,0,JAM1,0,12,NULL,"BACK     CTRL-B",&textKeys2};
static struct IntuiText textKeys0 =
  {1,0,JAM1,0,0,NULL,"ANS      CTRL-A",&textKeys1};


struct Gadget my_gadget=
{
  NULL,          /* NextGadget, no more gadgets in the list. */
  8,            /* LeftEdge, 40 pixels out. */
  16,            /* TopEdge, 20 lines down. */
  71,            /* Width, 71 pixels wide. */
  11,            /* Height, 11 pixels lines heigh. */
  GADGHCOMP,     /* Flags, when this gadget is highlighted, the gadget */
                 /* will be rendered in the complement colours. */
                 /* (Colour 0 (00) will be changed to colour 3 (11) */
                 /* (Colour 1 (01)           - " -           2 (10) */
                 /* (Colour 2 (10)           - " -           1 (01) */
                 /* (Colour 3 (11)           - " -           0 (00) */  
  0,
  BOOLGADGET,    /* GadgetType, a Boolean gadget. */
  NULL, /* GadgetRender, a pointer to our Border structure. */
                 /* (Since Intuition does not know if this will be a */
                 /* pointer to a Border structure or an Image structure, */
                 /* Intuition expects an APTR (normal memory pointer). */
                 /* We will therefore have to calm down the compiler by */
                 /* doing some "casting".) */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. (We complement the */
                 /* colours instead) */
  &textKeys0,      /* GadgetText, a pointer to our IntuiText structure. */
                 /* (See chapter 3 GRAPHICS for more information) */
  NULL,          /* MutualExclude, no mutual exclude. */
  NULL,          /* SpecialInfo, NULL since this is a Boolean gadget. */
                 /* (It is not a Proportional/String or Integer gdget) */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};

struct NewWindow windowHelpKeysLayout = {
  479, 16,
  136, 236,
  0,1,
  IDCMP_CLOSEWINDOW,
  WFLG_ACTIVATE|WFLG_CLOSEGADGET|WFLG_DRAGBAR,
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
  help_keys_is_open=1;
}

/**
 * Key help main
 */
void help_keys_main(void)
{
  if (help_keys_is_open==0)
    return;

  while (help_intuition_msg = (struct IntuiMessage *) GetMsg(windowHelpKeys->UserPort))
    {
      if (help_intuition_msg->Class==IDCMP_CLOSEWINDOW)
	{
	  ReplyMsg((struct Message *)help_intuition_msg);
	  CloseWindow(windowHelpKeys);
	  help_keys_is_open=0;
	  
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
