/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_print.h - Hang Up Requester
 */

#ifndef REQUESTER_PRINT_H
#define REQUESTER_PRINT_H

/**
 * Called when print requester needs to run
 */
void requester_print_show(void);

/** 
 * Called when print requester needs to stop.
 */
void requester_print_hide(void);

/**
 * requster_print_do(void)
 * Do the hang-up
 */
void requester_print_do(void);

#endif /* REQUESTER_PRINT_H */
