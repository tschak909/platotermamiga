/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_print.h - Device file requester
 */

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <graphics/displayinfo.h>
#include <exec/ports.h>
#include <clib/alib_protos.h>
#include "screen.h"
#include "io.h"
#include "requester_print.h"
#include "prefs.h"

#undef NULL
#define NULL 0

extern struct Window* myWindow;
extern struct Screen* myScreen;

union printerIO
{
  struct IOStdReq    ios;
  struct IODRPReq    iodrp;
  struct IOPrtCmdReq iopc;
};

SHORT printRequesterBorder_points[] = {
  0,0,
  111,0,
  111,31,
  0,31,
  0,0
};

/* The Border structure for the requester: */
struct Border printRequesterBorder=
{
  0, 0,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  printRequesterBorder_points, /* XY, pointer to the array with the coord. */
  NULL,        /* NextBorder, no other Border structures are connected. */
};

struct IntuiText printRequesterText =
  {1,0,JAM1,16,12,NULL,"Printing..",NULL};

static struct Requester printRequester =
{
  NULL,              /* OlderRequester, used by Intuition. */
  264, 184,            /* LeftEdge, TopEdge, 40 pixels out, 20 lines down. */
  112, 32,          /* Width, Height, 320 pixels wide, 100 lines high. */
  0, 0,              /* RelLeft, RelTop, Since POINTREL flag is not set, */
                     /* Intuition ignores these values. */
  NULL,              /* ReqGadget, pointer to the first gadget. */
  &printRequesterBorder, /* ReqBorder, pointer to a Border structure. */
  &printRequesterText,   /* ReqText, pointer to a IntuiText structure. */
  0,              /* Flags, no flags set. */
  0,                 /* BackFill, draw everything on an orange backgr. */
  NULL,              /* ReqLayer, used by Intuition. Set to NULL. */
  NULL,              /* ReqPad1, used by Intuition. Set to NULL. */
  NULL,              /* ImageBMap, no predrawn Bitmap. Set to NULL. */
                     /*            (The PREDRAWN flag was not set) */
  NULL,              /* RWindow, used by Intuition. Set to NULL. */
  NULL               /* ReqPad2, used by Intuition. Set to NULL. */
};

struct MsgPort* printerMP;
struct PrinterData *PD;
struct PrinterExtendedData *PED;
union printerIO *PIO;
LONG modeID, i,j;
ULONG dcol[5], drow[5];
ULONG signal;

/**
 * Called when print requester needs to run
 */
void requester_print_show(void)
{
  Request(&printRequester,myWindow);
}

/** 
 * Called when print requester needs to stop.
 */
void requester_print_hide(void)
{
  EndRequest(&printRequester,myWindow);
}

/**
 * requster_print_do(void)
 * Do the hang-up
 */
void requester_print_do(void)
{
  printerMP=CreatePort(0L,0L);

  if (printerMP)
    {
      PIO = (union printerIO *)CreateExtIO(printerMP, sizeof(union printerIO));
      if (PIO)
	{
	  // Everything open, do the dump.
	  if (!(OpenDevice("printer.device",0,(struct IORequest *)PIO,0)))
	    {
	      PD = (struct PrinterData *)PIO->iodrp.io_Device;
	      PED = (struct PrinterExtendedData *)&PD->pd_SegmentData->ps_PED;
	      /* Fill in those parts of the IODRPRequest which won't change */
	      PIO->iodrp.io_Command = PRD_DUMPRPORT;
	      PIO->iodrp.io_RastPort = &myScreen->RastPort;
	      PIO->iodrp.io_ColorMap = myScreen->ViewPort.ColorMap;
	      PIO->iodrp.io_Modes = 0x8004; // Hires laced.
	      PIO->iodrp.io_SrcX = myScreen->LeftEdge;
	      PIO->iodrp.io_SrcY = myScreen->TopEdge;
	      PIO->iodrp.io_SrcWidth = myScreen->Width;
	      PIO->iodrp.io_SrcHeight = myScreen->Height;
	      PIO->iodrp.io_DestCols = 0;
	      PIO->iodrp.io_DestRows = 0;
	      PD->pd_Preferences.PrintDensity = 7;
	      PIO->iodrp.io_Special = SPECIAL_ASPECT;
	      requester_print_show();
	      DoIO((struct IORequest *)PIO);
	      requester_print_hide();
	    }
	  else
	    {
	      printf("Couldnt open device\n");
	      // Couldn't open device.
	      DeleteExtIO((struct IORequest *)PIO);
	      DeletePort(printerMP);
	      return;
	    }
	}
      else
	{
	  printf("Couldnt create extio\n");
	  // Couldn't create extio request
	  DeletePort(printerMP);
	  return;
	}
    }
  else
    {
      printf("Couldnt create messageport.\n");
      // Couldn't create message port
      return;
    }

  printf("We printededed.");
  CloseDevice((struct IORequest *)PIO);
  DeleteExtIO((struct IORequest *)PIO);
  DeletePort(printerMP);
  
}
