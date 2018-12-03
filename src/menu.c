/* Menu definition */
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <intuition/intuition.h>

#define WIDTH_BAUD_MENU_ITEM      72
#define WIDTH_BAUD_MENU_TITLE     80
#define WIDTH_EDIT_MENU_ITEM      140
#define WIDTH_EDIT_MENU_TITLE     80
#define WIDTH_TERMINAL_MENU_ITEM  144
#define WIDTH_TERMINAL_MENU_TITLE 80
#define WIDTH_HELP_MENU_ITEM      180
#define WIDTH_HELP_MENU_TITLE     80
#define HEIGHT_ITEM               12

#define LEFT_TEXT_ITEM             4
#define TOP_TEXT_ITEM              2

#define MUTUALEXCLUDE_BAUD_MENU   0x1FF    // Baud rate radio

#define PEN_TEXT_BG    0
#define PEN_TEXT_FG    1

extern struct Window *myWindow; /* screen.c */

/**
 * Serial Menu
 */
static struct IntuiText textRTSCTS =
  {0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "RTS/CTS", NULL};

static struct MenuItem itemRTSCTS =
  { NULL, 0, 156, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|MENUTOGGLE|CHECKIT, 0, &textRTSCTS, &textRTSCTS, NULL,    NULL, 0 };

static struct IntuiText textsep3 = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "-----------", NULL };

static struct MenuItem itemsep3 =
  { &itemRTSCTS, 0, 144, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED, 0, &textsep3, &textsep3, NULL,    NULL, 0 };

static struct IntuiText text115200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "115200", NULL };

static struct MenuItem item115200 =
  { &itemsep3, 0, 132, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text115200, &text115200, NULL,    NULL, 0 };

static struct IntuiText text57600 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "57600", NULL };

static struct MenuItem item57600 =
  { &item115200, 0, 120, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text57600, &text57600, NULL,    NULL, 0 };

static struct IntuiText text38400 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "38400", NULL };

static struct MenuItem item38400 =
  { &item57600, 0, 108, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text38400, &text38400, NULL,    NULL, 0 };

static struct IntuiText text19200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "19200", NULL };

static struct MenuItem item19200 =
  { &item38400, 0, 96, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text19200, &text19200, NULL,    NULL, 0 };

static struct IntuiText text9600 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "9600", NULL };

static struct MenuItem item9600 =
  { &item19200, 0, 84, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text9600, &text9600, NULL,    NULL, 0 };

static struct IntuiText text4800 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "4800", NULL };

static struct MenuItem item4800 =
  { &item9600, 0, 72, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text4800, &text4800, NULL,    NULL, 0 };

static struct IntuiText text2400 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "2400", NULL };

static struct MenuItem item2400 =
  { &item4800, 0, 60, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text2400, &text2400, NULL,    NULL, 0 };

static struct IntuiText text1200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "1200", NULL };

static struct MenuItem item1200 =
  { &item2400, 0, 48, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text1200, &text1200, NULL,    NULL, 0 };

static struct IntuiText text300 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "300", NULL };

static struct MenuItem item300 =
  { &item1200, 0, 36, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text300, &text300, NULL,    NULL, 0 };

static struct IntuiText textBaudGroup =
  {0, 1, JAM2, 0, TOP_TEXT_ITEM, NULL, "Baud", NULL};

static struct MenuItem itemBaudGroup =
  { &item300, 0, 24, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED, 0, &textBaudGroup, &textBaudGroup, NULL,    NULL, 0 };

static struct IntuiText textsep2 = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "-----------", NULL };

static struct MenuItem itemsep2 =
  { &itemBaudGroup, 0, 12, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED, 0, &textsep2, &textsep2, NULL,    NULL, 0 };

static struct IntuiText textDevice =
  {0, 1, JAM2, 0, TOP_TEXT_ITEM, NULL, "Device...", NULL};

static struct MenuItem itemDevice =
  { &itemsep2, 0, 0, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP, 0, &textDevice, &textDevice, NULL,    NULL, 0 };


/**
 * Edit Menu
 */

static struct IntuiText textCopyScreen =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Copy Screen", NULL};

static struct MenuItem itemCopyScreen =
  { NULL, 0, 24, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textCopyScreen, &textCopyScreen, 'A',    NULL, 0 };

static struct IntuiText textPasteText =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Paste Text", NULL};

static struct MenuItem itemPasteText =
  { &itemCopyScreen, 0, 12, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textPasteText, &textPasteText, 'V',    NULL, 0 };

static struct IntuiText textCopyText =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Copy Text", NULL};

static struct MenuItem itemCopyText =
  { &itemPasteText, 0, 0, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textCopyText, &textCopyText, 'C',    NULL, 0 };


/**
 * Terminal Menu
 */

static struct IntuiText textQuit = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Quit", NULL };

static struct MenuItem itemQuit =
  { NULL, 0, 36, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ, 0, &textQuit, &textQuit, 'X',    NULL, 0 };


static struct IntuiText textAbout = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "About...", NULL };

static struct MenuItem itemAbout =
  { &itemQuit, 0, 24, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP, 0, &textAbout, &textAbout, NULL,    NULL, 0 };

static struct IntuiText textsep1 = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "-----------------", NULL };

static struct MenuItem itemsep1 =
  { &itemAbout, 0, 12, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED, 0, &textsep1, &textsep1, NULL,    NULL, 0 };

static struct IntuiText textPLATOMode =
  {0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "PLATO Mode", NULL};

static struct MenuItem itemPLATOMode =
  { &itemsep1, 0, 0, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ|MENUTOGGLE|CHECKIT, 0, &textPLATOMode, &textPLATOMode, 'T',    NULL, 0 };

/**
 * Help menu
 */

static struct IntuiText textPaletteDebug =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Show Palette...", NULL};

static struct MenuItem itemPaletteDebug =
  { NULL, 0, 12, WIDTH_HELP_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ, 0, &textPaletteDebug, &textPaletteDebug, 'L',    NULL, 0 };

static struct IntuiText textHelpKeys =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "PLATO Keys...", NULL};

static struct MenuItem itemHelpKeys =
  { &itemPaletteDebug, 0, 0, WIDTH_HELP_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ, 0, &textHelpKeys, &textHelpKeys, 'K',    NULL, 0 };

/**
 * The menu strip 
 */

static struct Menu menuHelp =
  { NULL, 200, 0, 46, HEIGHT_ITEM, MENUENABLED, "Help", &itemHelpKeys, 0, 0, 0, 0    };
  

static struct Menu menuSerial =
  { &menuHelp, 130, 0, 60, HEIGHT_ITEM, MENUENABLED, "Serial", &itemDevice, 0, 0, 0, 0    };

static struct Menu menuEdit =
  { &menuSerial, 80, 0, 48, HEIGHT_ITEM, MENUENABLED, "Edit", &itemCopyText, 0, 0, 0, 0    };


struct Menu menuTerminal = 
  { &menuEdit, 0, 0, 72, HEIGHT_ITEM, MENUENABLED, "Terminal", &itemPLATOMode, 0, 0, 0, 0    };

/**
 * toggle PLATO mode checkbox
 */
void menu_toggle_plato_mode(unsigned char toggle)
{
  /* if (!myWindow) */
  /*   return; */

  /* ClearMenuStrip(myWindow); */
  /* if (toggle==1) */
  /*   itemPLATOMode.Flags=ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ|MENUTOGGLE|CHECKIT|CHECKED; */
  /* else */
  /*   itemPLATOMode.Flags=ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ|MENUTOGGLE|CHECKIT; */
  /* ResetMenuStrip(myWindow,&menuTerminal); */
}

/* Add our menu to the menustrip */
void menu_init() {
  SetMenuStrip(myWindow,&menuTerminal);
}
