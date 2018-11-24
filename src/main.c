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
#define true 1
#define false 0

unsigned char already_started=false;

extern MySer* ms;
extern struct Window* myWindow;
 
int main(void)
{
  screen_init();
  menu_init();
  io_init();
  sound_init();
  touch_init();
  terminal_init();
  ShowPLATO(splash,sizeof(splash));
  terminal_initial_position();
  for (;;)
    {
      Wait((1L << ms->readport->mp_SigBit) | ( 1L << myWindow->UserPort->mp_SigBit));
      io_main();
      keyboard_main();
      touch_main();
    }
}

/**
 * Exit the program as gracefully as possible.
 */
void done(void)
{
  touch_done();
  io_done();
  sound_done();
  screen_done();
  exit(0); /* TODO: add exit code? */
}
