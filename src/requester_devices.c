/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_devices.h - Device file requester
 */

#include <intuition/intuition.h>
#include <clib/intuition_protos.h>
#include "requester_devices.h"

SHORT unit_string_border_points[]=
{
   -7, -4, /* Start at position (-7, -4) */
  36, -4, /* Draw a line to the right to position (200,-4) */
  36, 11, /* Draw a line down to position (200,11) */
   -7, 11, /* Draw a line to the left to position (-7,11) */
   -7, -4  /* Finish of by drawing a line up to position (-7,-4) */ 
};

/* The Border structure for the string gadget: */
struct Border unit_string_border=
{
  0, 0,                 /* LeftEdge, TopEdge. */
  1,                    /* FrontPen, colour register 1. */
  0,                    /* BackPen, for the moment unused. */
  JAM1,                 /* DrawMode, draw the lines with colour 1. */
  5,                    /* Count, 5 pair of coordinates in the array. */
  unit_string_border_points, /* XY, pointer to the array with the coordinates. */
  NULL,                 /* NextBorder, no other Border structures. */
};



/* The IntuiText structure for the string gadget: */
struct IntuiText unit_string_text=
{
  1,         /* FrontPen, colour register 1. (white) */
  0,         /* BackPen, not used since JAM1. */
  JAM1,      /* DrawMode, draw the characters with colour 1, and do not */
             /* bother about the background. */ 
  -53, 0,    /* LeftEdge, TopEdge. */
  NULL,      /* ITextFont, use default font. */
  "Unit:",   /* IText, the text that will be printed. */
  NULL,      /* NextText, no other IntuiText structures. */
};



UBYTE unit_name_buffer[8]="0"; /* 50 characters including the NULL-sign. */
UBYTE unit_name_undo_buffer[8]; /* Must be at least as big as unit_name_buffer. */



struct StringInfo unit_string_info=
{
  unit_name_buffer,       /* Buffer, pointer to a null-terminated string. */
  unit_name_undo_buffer,  /* UndoBuffer, pointer to a null-terminated string. */
                   /* (Remember unit_name_buffer is equal to &unit_name_buffer[0]) */
  0,               /* BufferPos, initial position of the cursor. */
  8,              /* MaxChars, 50 characters + null-sign ('\0'). */
  0,               /* DispPos, first character in the string should be */
                   /* first character in the display. */

  /* Intuition initializes and maintaines these variables: */

  0,               /* UndoPos */
  0,               /* NumChars */
  0,               /* DispCount */
  0, 0,            /* CLeft, CTop */
  NULL,            /* LayerPtr */
  NULL,            /* LongInt */
  NULL,            /* AltKeyMap */
};


struct Gadget unit_string_gadget=
{
  NULL,          /* NextGadget, no more gadgets in the list. */
  68,            /* LeftEdge, 68 pixels out. */
  46,            /* TopEdge, 26 lines down. */
  36,           /* Width, 198 pixels wide. */
  8,             /* Height, 8 pixels lines heigh. */
  GADGHCOMP,     /* Flags, draw the select box in the complement */
                 /* colours. Note: it actually only the cursor which */
                 /* will be drawn in the complement colours (yellow). */
                 /* If you set the flag GADGHNONE the cursor will not be */
                 /* highlighted, and the user will therefore not be able */
                 /* to see it. */
  GADGIMMEDIATE| /* Activation, our program will recieve a message when */
  RELVERIFY,     /* the user has selected this gadget, and when the user */
                 /* has released it. */ 
  STRGADGET|     /* GadgetType, a String gadget which is connected to */
  REQGADGET,     /* a requester. IMPORTANT! Every gadget which is */
                 /* connectd to a requester must have the REQGADGET flsg */
                 /* set in the GadgetType field. */
  (APTR) &unit_string_border, /* GadgetRender, a pointer to our Border struc. */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. */
  &unit_string_text,  /* GadgetText, a pointer to our IntuiText structure. */
  NULL,          /* MutualExclude, no mutual exclude. */
  (APTR) &unit_string_info, /* SpecialInfo, a pointer to a StringInfo str. */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};


/* The coordinates for the box around the string gadget: */
SHORT device_string_border_points[]=
{
   -7, -4, /* Start at position (-7, -4) */
  200, -4, /* Draw a line to the right to position (200,-4) */
  200, 11, /* Draw a line down to position (200,11) */
   -7, 11, /* Draw a line to the left to position (-7,11) */
   -7, -4  /* Finish of by drawing a line up to position (-7,-4) */ 
};

/* The Border structure for the string gadget: */
struct Border device_string_border=
{
  0, 0,                 /* LeftEdge, TopEdge. */
  1,                    /* FrontPen, colour register 1. */
  0,                    /* BackPen, for the moment unused. */
  JAM1,                 /* DrawMode, draw the lines with colour 1. */
  5,                    /* Count, 5 pair of coordinates in the array. */
  device_string_border_points, /* XY, pointer to the array with the coordinates. */
  NULL,                 /* NextBorder, no other Border structures. */
};



/* The IntuiText structure for the string gadget: */
struct IntuiText device_string_text=
{
  1,         /* FrontPen, colour register 1. (white) */
  0,         /* BackPen, not used since JAM1. */
  JAM1,      /* DrawMode, draw the characters with colour 1, and do not */
             /* bother about the background. */ 
  -53, 0,    /* LeftEdge, TopEdge. */
  NULL,      /* ITextFont, use default font. */
  "Name:",   /* IText, the text that will be printed. */
  NULL,      /* NextText, no other IntuiText structures. */
};



UBYTE device_name_buffer[50]="serial.device"; /* 50 characters including the NULL-sign. */
UBYTE device_name_undo_buffer[50]; /* Must be at least as big as device_name_buffer. */



struct StringInfo device_string_info=
{
  device_name_buffer,       /* Buffer, pointer to a null-terminated string. */
  device_name_undo_buffer,  /* UndoBuffer, pointer to a null-terminated string. */
                   /* (Remember device_name_buffer is equal to &device_name_buffer[0]) */
  0,               /* BufferPos, initial position of the cursor. */
  50,              /* MaxChars, 50 characters + null-sign ('\0'). */
  0,               /* DispPos, first character in the string should be */
                   /* first character in the display. */

  /* Intuition initializes and maintaines these variables: */

  0,               /* UndoPos */
  0,               /* NumChars */
  0,               /* DispCount */
  0, 0,            /* CLeft, CTop */
  NULL,            /* LayerPtr */
  NULL,            /* LongInt */
  NULL,            /* AltKeyMap */
};


struct Gadget device_string_gadget=
{
  &unit_string_gadget,          /* NextGadget, no more gadgets in the list. */
  68,            /* LeftEdge, 68 pixels out. */
  26,            /* TopEdge, 26 lines down. */
  198,           /* Width, 198 pixels wide. */
  8,             /* Height, 8 pixels lines heigh. */
  GADGHCOMP,     /* Flags, draw the select box in the complement */
                 /* colours. Note: it actually only the cursor which */
                 /* will be drawn in the complement colours (yellow). */
                 /* If you set the flag GADGHNONE the cursor will not be */
                 /* highlighted, and the user will therefore not be able */
                 /* to see it. */
  GADGIMMEDIATE| /* Activation, our program will recieve a message when */
  RELVERIFY,     /* the user has selected this gadget, and when the user */
                 /* has released it. */ 
  STRGADGET|     /* GadgetType, a String gadget which is connected to */
  REQGADGET,     /* a requester. IMPORTANT! Every gadget which is */
                 /* connectd to a requester must have the REQGADGET flsg */
                 /* set in the GadgetType field. */
  (APTR) &device_string_border, /* GadgetRender, a pointer to our Border struc. */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. */
  &device_string_text,  /* GadgetText, a pointer to our IntuiText structure. */
  NULL,          /* MutualExclude, no mutual exclude. */
  (APTR) &device_string_info, /* SpecialInfo, a pointer to a StringInfo str. */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};

extern struct Window* myWindow;

/* The coordinates for the box: */

SHORT buttonBorderBold_points[]=
{
   -1,  -1, /* Start at position (0,0) */
  71,  1, /* Draw a line to the right to position (70,0) */
  71, 21, /* Draw a line down to position (70,10) */
   -1, 21, /* Draw a line to the right to position (0,10) */
   -1,  -1  /* Finish of by drawing a line up to position (0,0) */ 
};

SHORT buttonBorder_points[]=
{
   0,  0, /* Start at position (0,0) */
  70,  0, /* Draw a line to the right to position (70,0) */
  70, 20, /* Draw a line down to position (70,10) */
   0, 20, /* Draw a line to the right to position (0,10) */
   0,  0  /* Finish of by drawing a line up to position (0,0) */ 
};

/* The Border structure: */
struct Border buttonBorder=
{
  0, 0,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  buttonBorder_points, /* XY, pointer to the array with the coord. */
  NULL,        /* NextBorder, no other Border structures are connected. */
};

/* The Border structure: */
struct Border buttonBorderBold=
{
  1, 1,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  buttonBorder_points, /* XY, pointer to the array with the coord. */
  &buttonBorder,        /* NextBorder, no other Border structures are connected. */
};

struct IntuiText applyText=
{
  1,         /* FrontPen, colour register 1. */
  0,         /* BackPen, colour register 0. */
  JAM1,      /* DrawMode, draw the characters with colour 1, do not */
             /* change the background. */ 
  14, 6,      /* LeftEdge, TopEdge. */
  NULL,      /* ITextFont, use default font. */
  "APPLY",/* IText, the text that will be printed. */
  NULL,      /* NextText, no other IntuiText structures are connected. */
};

struct IntuiText cancelText=
{
  1,         /* FrontPen, colour register 1. */
  0,         /* BackPen, colour register 0. */
  JAM1,      /* DrawMode, draw the characters with colour 1, do not */
             /* change the background. */ 
  14, 6,      /* LeftEdge, TopEdge. */
  NULL,      /* ITextFont, use default font. */
  "CANCEL",   /* IText, the text that will be printed. */
  NULL,      /* NextText, no other IntuiText structures are connected. */
};

struct Gadget devicesRequesterGadget_cancel =
{
  &device_string_gadget,          /* NextGadget, no more gadgets in the list. */
  80,            /* LeftEdge, 40 pixels out. */
  70,            /* TopEdge, 20 lines down. */
  71,            /* Width, 71 pixels wide. */
  21,            /* Height, 11 pixels lines heigh. */
  GADGHCOMP,     /* Flags, when this gadget is highlighted, the gadget */
                 /* will be rendered in the complement colours. */
                 /* (Colour 0 (00) will be changed to colour 3 (11) */
                 /* (Colour 1 (01)           - " -           2 (10) */
                 /* (Colour 2 (10)           - " -           1 (01) */
                 /* (Colour 3 (11)           - " -           0 (00) */  
  GADGIMMEDIATE| /* Activation, our program will recieve a message when */
  RELVERIFY|     /* the user has selected this gadget, and when the user */
                 /* has released it. */
  ENDGADGET,     /* When the user has selected this gadget, the */
                 /* requester is satisfied, and is deactivated. */
                 /* IMPORTANT! At least one gadget per requester */
                 /* must have the flag ENDGADGET set. If not, the */
                 /* requester would never be deactivated! */

  BOOLGADGET|    /* GadgetType, a Boolean gadget which is connected to */
  REQGADGET,     /* a requester. IMPORTANT! Every gadget which is */
                 /* connectd to a requester must have the REQGADGET flsg */
                 /* set in the GadgetType field. */
  (APTR) &buttonBorder, /* GadgetRender, a pointer to our Border struc. */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. (We complement the */
                 /* colours instead) */
  &cancelText,  /* GadgetText, a pointer to our IntuiText structure. */
                 /* (See chapter 3 GRAPHICS for more information) */
  NULL,          /* MutualExclude, no mutual exclude. */
  NULL,          /* SpecialInfo, NULL since this is a Boolean gadget. */
                 /* (It is not a Proportional/String or Integer gdget) */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};

struct Gadget devicesRequesterGadget_apply=
{
  &devicesRequesterGadget_cancel,          /* NextGadget, no more gadgets in the list. */
  160,            /* LeftEdge, 40 pixels out. */
  70,            /* TopEdge, 20 lines down. */
  71,            /* Width, 71 pixels wide. */
  21,            /* Height, 11 pixels lines heigh. */
  GADGHCOMP,     /* Flags, when this gadget is highlighted, the gadget */
                 /* will be rendered in the complement colours. */
                 /* (Colour 0 (00) will be changed to colour 3 (11) */
                 /* (Colour 1 (01)           - " -           2 (10) */
                 /* (Colour 2 (10)           - " -           1 (01) */
                 /* (Colour 3 (11)           - " -           0 (00) */  
  GADGIMMEDIATE| /* Activation, our program will recieve a message when */
  RELVERIFY|     /* the user has selected this gadget, and when the user */
                 /* has released it. */
  ENDGADGET,     /* When the user has selected this gadget, the */
                 /* requester is satisfied, and is deactivated. */
                 /* IMPORTANT! At least one gadget per requester */
                 /* must have the flag ENDGADGET set. If not, the */
                 /* requester would never be deactivated! */

  BOOLGADGET|    /* GadgetType, a Boolean gadget which is connected to */
  REQGADGET,     /* a requester. IMPORTANT! Every gadget which is */
                 /* connectd to a requester must have the REQGADGET flsg */
                 /* set in the GadgetType field. */
  (APTR) &buttonBorderBold, /* GadgetRender, a pointer to our Border struc. */
  NULL,          /* SelectRender, NULL since we do not supply the gadget */
                 /* with an alternative image. (We complement the */
                 /* colours instead) */
  &applyText,  /* GadgetText, a pointer to our IntuiText structure. */
                 /* (See chapter 3 GRAPHICS for more information) */
  NULL,          /* MutualExclude, no mutual exclude. */
  NULL,          /* SpecialInfo, NULL since this is a Boolean gadget. */
                 /* (It is not a Proportional/String or Integer gdget) */
  0,             /* GadgetID, no id. */
  NULL           /* UserData, no user data connected to the gadget. */
};

/***********************************************************************/
/* Important notice:                                                   */
/* Remember that every gadget which is connected to a requester must   */
/* have the flag REQGADGET set in the GadgetType field. Remember also  */
/* that at least one gadget per requester must have the ENDGADGET flag */
/* set in the Activation field.                                        */
/***********************************************************************/

/************************************/
/* THE BORDER AROUND THE REQUESTER: */
/************************************/

/* The coordinates for the box around the requester: */
SHORT devicesRequesterBorder_points[]=
{
    0,  0, /* Start at position (0,0) */
  319,  0, /* Draw a line to the right to position (319,0) */
  319, 99, /* Draw a line down to position (319,99) */
    0, 99, /* Draw a line to the right to position (319,99) */
    0,  0  /* Finish of by drawing a line up to position (0,0) */ 
};

/* The Border structure for the requester: */
struct Border devicesRequesterBorder=
{
  0, 0,        /* LeftEdge, TopEdge. */
  1,           /* FrontPen, colour register 1. */
  0,           /* BackPen, for the moment unused. */
  JAM1,        /* DrawMode, draw the lines with colour 1. */
  5,           /* Count, 5 pair of coordinates in the array. */
  devicesRequesterBorder_points, /* XY, pointer to the array with the coord. */
  NULL,        /* NextBorder, no other Border structures are connected. */
};



/**********************************/
/* THE TEXT INSIDE THE REQUESTER: */
/**********************************/

struct IntuiText devicesRequesterText =
  {1,0,JAM1,30,8,NULL,"Enter the name of a serial device",NULL};

static struct Requester devicesRequester =
{
  NULL,              /* OlderRequester, used by Intuition. */
  140, 120,            /* LeftEdge, TopEdge, 40 pixels out, 20 lines down. */
  320, 100,          /* Width, Height, 320 pixels wide, 100 lines high. */
  0, 0,              /* RelLeft, RelTop, Since POINTREL flag is not set, */
                     /* Intuition ignores these values. */
  &devicesRequesterGadget_apply, /* ReqGadget, pointer to the first gadget. */
  &devicesRequesterBorder, /* ReqBorder, pointer to a Border structure. */
  &devicesRequesterText,   /* ReqText, pointer to a IntuiText structure. */
  NULL,              /* Flags, no flags set. */
  0,                 /* BackFill, draw everything on an orange backgr. */
  NULL,              /* ReqLayer, used by Intuition. Set to NULL. */
  NULL,              /* ReqPad1, used by Intuition. Set to NULL. */
  NULL,              /* ImageBMap, no predrawn Bitmap. Set to NULL. */
                     /*            (The PREDRAWN flag was not set) */
  NULL,              /* RWindow, used by Intuition. Set to NULL. */
  NULL               /* ReqPad2, used by Intuition. Set to NULL. */
};


/**
 * Called when devices requester needs to run
 */
void requester_devices_run(void)
{
  Request(&devicesRequester,myWindow);
}

/** 
 * Called when devices requester needs to stop.
 */
void requester_devices_done(void)
{
}
