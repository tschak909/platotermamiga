/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * prefs.h - Preferences file functions
 */

typedef struct _configInfo
{
  unsigned long io_Baud;              /* Baud Rate from IOExtSer */
  unsigned long io_RBufLen;           /* Read buffer size from IOExtSer */
  unsigned long rtscts_enabled;       /* RTS/CTS enabled? */
  unsigned char device_name[30];      /* Serial device name */
  unsigned long unit_number;          /* unit number */
  unsigned char paint_enabled;        /* is paint enabled? */
  unsigned char close_workbench;      /* Close the Workbench? */
} ConfigInfo;

extern ConfigInfo config;

/**
 * prefs_open() - Open preferences file
 */
void prefs_load(void);

/**
 * prefs_save() - Save the preferences file
 */
void prefs_save(void);

/**
 * Set default preferences
 */
void prefs_set_defaults(void);
