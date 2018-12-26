/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * clipboard.h - Clipboard functions.
 */

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "protocol.h"

/**
 * Initialize the clipboard (set up text buffer)
 */
void clipboard_init(void);

/**
 * Clipboard Clear
 */
void clipboard_clear(void);

/**
 * translate PLATO X coordinate (512) to nearest clipboard coordinate (64)
 */
unsigned char clipboard_coord_x(unsigned short x);

/**
 * translate PLATO X coordinate (512) to nearest clipboard coordinate (32)
 */
unsigned char clipboard_coord_y(unsigned short x);

/**
 * Write text to clipboard
 */
unsigned char clipboard_update_buffer(padPt* Coord, unsigned char* ch, unsigned char count);

/**
 * Write 4 bytes to clipboard device. Used for headers
 */
void clipboard_write_long(long* data);

/**
 * Write text to clipboard
 */
void clipboard_write(void);

/**
 * Write screen to clipboard
 */
void clipboard_write_screen(void);

/**
 * Tear down the clipboard
 */
void clipboard_done(void);

#endif /* CLIPBOARD_H */
