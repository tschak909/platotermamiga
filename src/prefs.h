/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * prefs.h - Preferences file functions
 */

#ifndef PREFS_H
#define PREFS_H

typedef struct configInfo
{
  unsigned long io_Baud;     /* Baud Rate from IOExtSer */
  unsigned long io_RBufLen;  /* Read buffer size from IOExtSer */
  unsigned long io_WriteLen; /* Write buffer size from IOExtSer */
  unsigned char device_name[128];     /* Serial device name */
  unsigned char paint_enabled;        /* is paint enabled? */
  unsigned char close_workbench;      /* Close the Workbench? */
} ConfigInfo;


/**
 * prefs_open() - Open preferences file
 */
void prefs_open(void);

/**
 * prefs_close() - Close the preferences file
 */
void prefs_close(void);

/**
 * prefs_save() - Save the preferences file
 */
void prefs_save(void);

/**
 * prefs_set_defaults() - Set default preferences
 */
void prefs_set_defaults(void);

#endif /* PREFS_H */
