#include <clib/exec_protos.h>
#include <proto/intuition.h>
#include <stdlib.h>
#include "screen.h"
#include "io.h"
#include "sound.h"
#include "terminal.h"
#include "splash.h"
#include "keyboard.h"
#include "touch.h"
#include "menu.h"
#include "help.h"
#include "palette_debug.h"
#include "prefs.h"
#include "clipboard.h"

#define true 1
#define false 0

char version[]="$VER: PLATOTerm 0.3 ("__DATE__")";
unsigned char already_started=false;
unsigned long sigbits;

extern MySer* ms;
extern struct Window* myWindow;
 
int main(void)
{
  prefs_load();
  clipboard_init();
  screen_init();
  menu_init();
  sound_init();
  io_init();
  touch_init();
  terminal_init();
  ShowPLATO(splash,sizeof(splash));
  terminal_initial_position();
  sigbits=(1L << ms->readport->mp_SigBit) | ( 1L << myWindow->UserPort->mp_SigBit);
  for (;;)
    {
      Wait(sigbits);
      io_main();
      keyboard_main();
    }
}

/**
 * Exit the program as gracefully as possible.
 */
void done(void)
{
  clipboard_done();
  help_done();
  palette_debug_done();
  io_done();
  sound_done();
  menu_done();
  screen_done();
  exit(0); /* TODO: add exit code? */
}
