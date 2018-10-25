/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * screen.h - Display output functions
 */

#include "screen.h"
#include "protocol.h"

unsigned char CharWide=8;
unsigned char CharHigh=16;
unsigned char screen_mode;
extern padBool FastText; /* protocol.c */
padPt TTYLoc;
unsigned char FONT_SIZE_X;
unsigned char FONT_SIZE_Y;
unsigned short* scalex;
unsigned short* scaley;
unsigned char* font;
unsigned short* fontptr;
unsigned short width;
unsigned short height;
unsigned char current_foreground;
unsigned char current_background;
unsigned char fontm23[2048];
unsigned char is_mono=0;


/**
 * screen_init() - Set up the screen
 */
void screen_init(void)
{
}

/**
 * screen_load_driver()
 * Load the TGI driver
 */
void screen_load_driver(void)
{
}

/**
 * screen_init_hook()
 * Called after tgi_init to set any special features, e.g. nmi trampolines.
 */
void screen_init_hook(void)
{
}


/**
 * screen_update_colors() - Set the terminal colors
 */
void screen_update_colors(void)
{
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
}

/**
 * screen_block_draw(Coord1, Coord2) - Perform a block fill from Coord1 to Coord2
 */
void screen_block_draw(padPt* Coord1, padPt* Coord2)
{
}

/**
 * screen_dot_draw(Coord) - Plot a mode 0 pixel
 */
void screen_dot_draw(padPt* Coord)
{
}

/**
 * screen_line_draw(Coord1, Coord2) - Draw a mode 1 line
 */
void screen_line_draw(padPt* Coord1, padPt* Coord2)
{
}

/**
 * screen_char_draw(Coord, ch, count) - Output buffer from ch* of length count as PLATO characters
 */
void screen_char_draw(padPt* Coord, unsigned char* ch, unsigned char count)
{
}

/**
 * screen_tty_char - Called to plot chars when in tty mode
 */
void screen_tty_char(padByte theChar)
{
}

/**
 * screen_foreground - set foreground color
 */
void screen_foreground(padRGB* theColor)
{
}

/**
 * screen_background - set background_color
 */
void screen_background(padRGB* theColor)
{
}

/**
 * screen_paint - Paint the screen
 * Calling convention is different due to this needing to be recursive.
 */
void screen_paint(padPt* Coord)
{
}

/**
 * screen_done()
 * Close down TGI
 */
void screen_done(void)
{
}
