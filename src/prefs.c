/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * prefs.c - Preferences file functions
 */

#ifndef PREFS_H
#define PREFS_H

#define PREFS_FILE "PLATOTermPrefs"

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <string.h>
#include <clib/alib_stdio_protos.h>
#include "prefs.h"

ConfigInfo config;
BPTR fileHandle;

/**
 * prefs_set_defaults(void)
 */
void prefs_set_defaults(void)
{
  config.io_Baud=1200;
  config.io_RBufLen=4000;
  strcpy(config.device_name,"serial.device");
  config.unit_number=0;
  config.paint_enabled=1;
  config.close_workbench=0;
  config.rtscts_enabled=1;
  prefs_save();
}

/**
 * prefs_open() - Open preferences file
 */
void prefs_load(void)
{
  fileHandle=Open(PREFS_FILE,MODE_OLDFILE);
  if (fileHandle==0)
  {
    fileHandle=Open(PREFS_FILE,MODE_NEWFILE);
    Close(fileHandle);
    prefs_set_defaults();
  }
  else
    {
      Read(fileHandle,&config,sizeof(config));
      Close(fileHandle);
    }
}

/**
 * Format string for comment field.
 */
void prefs_comment(void)
{
  char comment[80]={"PLATOTerm Preferences File"};
  /* sprintf(comment,"%s:%d, %d baud, bufsize: %d, paint: %d wbench: %d", */
  /* 	  config.device_name, */
  /* 	  config.unit_number, */
  /* 	  config.io_Baud, */
  /* 	  config.io_RBufLen, */
  /* 	  config.paint_enabled, */
  /* 	  config.close_workbench); */
  SetComment(PREFS_FILE,comment);
}

/**
 * prefs_save() - Save the preferences file
 */
void prefs_save(void)
{
  fileHandle=Open(PREFS_FILE,MODE_READWRITE);

  if (fileHandle==0)
    return;

  Write(fileHandle,&config,sizeof(config));
  Close(fileHandle);
  prefs_comment();
}

#endif /* PREFS_H */
