/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * requester_devices.h - Device file requester
 */

#ifndef REQUESTER_DEVICES_H
#define REQUESTER_DEVICES_H

/**
 * Called when devices requester needs to run
 */
void requester_devices_run(void);

/** 
 * Called when devices requester needs to stop.
 */
void requester_devices_done(void);

/**
 * Called when the devices requester has been dismissed without the
 * settings being applied.
 */
void requester_devices_cancel(void);

#endif
