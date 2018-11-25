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
#define HEIGHT_ITEM               12

#define LEFT_TEXT_ITEM             4
#define TOP_TEXT_ITEM              2

#define MUTUALEXCLUDE_BAUD_MENU   0x1FF    // Baud rate radio

#define PEN_TEXT_BG    0
#define PEN_TEXT_FG    1


/**
 * Baud Menu
 */

struct IntuiText text115200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "115200", NULL };

struct MenuItem item115200 =
  { NULL, 0, 96, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text115200, &text115200, NULL,    NULL, 0 };

struct IntuiText text57600 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "57600", NULL };

struct MenuItem item57600 =
  { &item115200, 0, 84, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text57600, &text57600, NULL,    NULL, 0 };

struct IntuiText text38400 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "38400", NULL };

struct MenuItem item38400 =
  { &item57600, 0, 72, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text38400, &text38400, NULL,    NULL, 0 };

struct IntuiText text19200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "19200", NULL };

struct MenuItem item19200 =
  { &item38400, 0, 60, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text19200, &text19200, NULL,    NULL, 0 };

struct IntuiText text9600 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "9600", NULL };

struct MenuItem item9600 =
  { &item19200, 0, 48, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text9600, &text9600, NULL,    NULL, 0 };

struct IntuiText text4800 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "4800", NULL };

struct MenuItem item4800 =
  { &item9600, 0, 36, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text4800, &text4800, NULL,    NULL, 0 };

struct IntuiText text2400 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "2400", NULL };

struct MenuItem item2400 =
  { &item4800, 0, 24, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text2400, &text2400, NULL,    NULL, 0 };

struct IntuiText text1200 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "1200", NULL };

struct MenuItem item1200 =
  { &item2400, 0, 12, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text1200, &text1200, NULL,    NULL, 0 };

struct IntuiText text300 = 
  { 0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "300", NULL };

struct MenuItem item300 =
  { &item1200, 0, 0, WIDTH_BAUD_MENU_ITEM, HEIGHT_ITEM, CHECKIT|ITEMTEXT|ITEMENABLED|HIGHCOMP, MUTUALEXCLUDE_BAUD_MENU, &text300, &text300, NULL,    NULL, 0 };

/**
 * Edit Menu
 */

struct IntuiText textCopyScreen =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Copy Screen", NULL};

struct MenuItem itemCopyScreen =
  { NULL, 0, 24, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textCopyScreen, &textCopyScreen, 'A',    NULL, 0 };

struct IntuiText textPasteText =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Paste Text", NULL};

struct MenuItem itemPasteText =
  { &itemCopyScreen, 0, 12, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textPasteText, &textPasteText, 'V',    NULL, 0 };

struct IntuiText textCopyText =
  {0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Copy Text", NULL};

struct MenuItem itemCopyText =
  { &itemPasteText, 0, 0, WIDTH_EDIT_MENU_ITEM, HEIGHT_ITEM, ITEMENABLED|ITEMTEXT|HIGHCOMP|COMMSEQ, 0, &textCopyText, &textCopyText, 'C',    NULL, 0 };


/**
 * Terminal Menu
 */

struct IntuiText textQuit = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "Quit", NULL };

struct MenuItem itemQuit =
  { NULL, 0, 24, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ, 0, &textQuit, &textQuit, 'X',    NULL, 0 };

struct IntuiText textsep1 = 
  { 0, 1, JAM2, LEFT_TEXT_ITEM, TOP_TEXT_ITEM, NULL, "-----------------", NULL };

struct MenuItem itemsep1 =
  { &itemQuit, 0, 12, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED, 0, &textsep1, &textsep1, NULL,    NULL, 0 };

struct IntuiText textPLATOMode =
  {0, 1, JAM2, CHECKWIDTH, TOP_TEXT_ITEM, NULL, "PLATO Mode", NULL};

struct MenuItem itemPLATOMode =
  { &itemsep1, 0, 0, WIDTH_TERMINAL_MENU_ITEM, HEIGHT_ITEM, ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ|MENUTOGGLE|CHECKIT, 0, &textPLATOMode, &textPLATOMode, 'T',    NULL, 0 };

/**
 * The menu strip 
 */

struct Menu menuBaud =
  { NULL, 130, 0, 60, HEIGHT_ITEM, MENUENABLED, "Baud", &item300, 0, 0, 0, 0    };

struct Menu menuEdit =
  { &menuBaud, 80, 0, 48, HEIGHT_ITEM, MENUENABLED, "Edit", &itemCopyText, 0, 0, 0, 0    };


struct Menu menuTerminal = 
  { &menuEdit, 0, 0, 72, HEIGHT_ITEM, MENUENABLED, "Terminal", &itemPLATOMode, 0, 0, 0, 0    };

/* Add our menu to the menustrip */
void menu_init() {
  extern struct Window *myWindow;
  SetMenuStrip(myWindow,&menuTerminal);
}
