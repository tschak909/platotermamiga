#include <stdio.h>
#include "screen.h"
#include "io.h"
#include "terminal.h"
#include "splash.h"
#include "keyboard.h"
#include "touch.h"

#define true 1
#define false 0

unsigned char already_started=false;

int main(void)
{
  screen_init();
  io_init();
  touch_init();
  terminal_init();
  ShowPLATO(splash,sizeof(splash));
  terminal_initial_position();
  for (;;)
    {
      io_main();
      keyboard_main();
      touch_main();
    }
}
