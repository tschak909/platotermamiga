/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_hang_up.h - Device file requester
 */

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "screen.h"
#include "io.h"
#include "requester_hang_up.h"
#include "prefs.h"

#undef NULL
#define NULL 0

extern struct Window* myWindow;

SHORT hangUpRequesterBorder_points[] = {
  0,0,
  111,0,
  111,31,
  0,31,
  0,0
};

/* The Border structure for the requester: */
struct Border hangUpRequesterBorder=
{
  0, 0,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  hangUpRequesterBorder_points, /* XY, pointer to the array with the coord. */
  NULL,        /* NextBorder, no other Border structures are connected. */
};

struct IntuiText hangUpRequesterText =
  {1,0,JAM1,16,12,NULL,"Hanging Up",NULL};

static struct Requester hangUpRequester =
{
  NULL,              /* OlderRequester, used by Intuition. */
  264, 184,            /* LeftEdge, TopEdge, 40 pixels out, 20 lines down. */
  112, 32,          /* Width, Height, 320 pixels wide, 100 lines high. */
  0, 0,              /* RelLeft, RelTop, Since POINTREL flag is not set, */
                     /* Intuition ignores these values. */
  NULL,              /* ReqGadget, pointer to the first gadget. */
  &hangUpRequesterBorder, /* ReqBorder, pointer to a Border structure. */
  &hangUpRequesterText,   /* ReqText, pointer to a IntuiText structure. */
  0,              /* Flags, no flags set. */
  0,                 /* BackFill, draw everything on an orange backgr. */
  NULL,              /* ReqLayer, used by Intuition. Set to NULL. */
  NULL,              /* ReqPad1, used by Intuition. Set to NULL. */
  NULL,              /* ImageBMap, no predrawn Bitmap. Set to NULL. */
                     /*            (The PREDRAWN flag was not set) */
  NULL,              /* RWindow, used by Intuition. Set to NULL. */
  NULL               /* ReqPad2, used by Intuition. Set to NULL. */
};

/**
 * Called when hang_up requester needs to run
 */
void requester_hang_up_show(void)
{
  Request(&hangUpRequester,myWindow);
}

/** 
 * Called when hang_up requester needs to stop.
 */
void requester_hang_up_hide(void)
{
  EndRequest(&hangUpRequester,myWindow);
}

/**
 * requster_hang_up_do(void)
 * Do the hang-up
 */
void requester_hang_up_do(void)
{
  screen_clear();
  InitTTY();
  requester_hang_up_show();
  io_send_byte('+');
  io_send_byte('+');
  io_send_byte('+');
  Delay(150);
  io_send_byte('A');
  io_send_byte('T');
  io_send_byte('H');
  io_send_byte('0');
  io_send_byte('\r');
  io_send_byte('\n');
  requester_hang_up_hide();
}
