/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_devices.h - Hang Up Requester
 */

#ifndef REQUESTER_HANG_UP_H
#define REQUESTER_HANG_UP_H

/**
 * Called when hang_up requester needs to run
 */
void requester_hang_up_show(void);

/** 
 * Called when hang_up requester needs to stop.
 */
void requester_hang_up_hide(void);

/**
 * requster_hang_up_do(void)
 * Do the hang-up
 */
void requester_hang_up_do(void);

#endif /* REQUESTER_HANG_UP_H */
