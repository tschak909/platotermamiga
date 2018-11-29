/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * touch.c - Touchscreen functions
 */

#ifndef TOUCH_H
#define TOUCH_H

#include "protocol.h"

/**
 * touch_init() - Set up touch screen
 */
void touch_init(void);

/**
 * touch_allow - Set whether touchpanel is active or not.
 */
void touch_allow(padBool allow);

#endif /* TOUCH_H */
