/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * keyboard.h - Keyboard functions
 */

#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "keyboard.h"
#include "protocol.h"
#include "screen.h"
#include "io.h"
#include "key.h"
#include "menu.h"

struct IntuiMessage* intuition_msg;
extern struct Window *myWindow;
unsigned char shift_state=0;
extern void done(void);
/**
 * keyboard_out - If platoKey < 0x7f, pass off to protocol
 * directly. Otherwise, platoKey is an access key, and the
 * ACCESS key must be sent, followed by the particular
 * access key from PTAT_ACCESS.
 */
void keyboard_out(unsigned char platoKey)
{
  if (platoKey==0xff)
    return;
  
  if (platoKey>0x7F)
    {
      Key(ACCESS);
      Key(ACCESS_KEYS[platoKey-0x80]);
      return;
    }
  Key(platoKey);
  return;
}

/**
 * keyboard_main - Handle the keyboard presses
 */
void keyboard_main(void)
{
  /* Defines for determining menu selection */
  UWORD menuNumber;
  UWORD menuNum;
  UWORD itemNum;
  UWORD subNum;
  struct MenuItem *item;

  while (intuition_msg = (struct IntuiMessage *) GetMsg(myWindow->UserPort))
    {
      if (intuition_msg->Class == VANILLAKEY)
	{
	  if (intuition_msg->Qualifier & IEQUALIFIER_RCOMMAND)
	    {
	      if (intuition_msg->Code=='x')
		{
		  done();
		}
	    }
	  else if (intuition_msg->Code == 0x0D) // Special case for SHIFT-NEXT
	    {
	      if (intuition_msg->Qualifier & IEQUALIFIER_LSHIFT)
		keyboard_out(ctrl_shift_key_to_pkey[intuition_msg->Code]);
	      else if (intuition_msg->Qualifier & IEQUALIFIER_RSHIFT)
		keyboard_out(ctrl_shift_key_to_pkey[intuition_msg->Code]);
	      else
		keyboard_out(key_to_pkey[intuition_msg->Code]);		
	    }
	  else if (intuition_msg->Qualifier & IEQUALIFIER_CONTROL)
	    {
	      if (intuition_msg->Qualifier & IEQUALIFIER_LSHIFT)
		keyboard_out(ctrl_shift_key_to_pkey[intuition_msg->Code]);
	      else if (intuition_msg->Qualifier & IEQUALIFIER_RSHIFT)
		keyboard_out(ctrl_shift_key_to_pkey[intuition_msg->Code]);
	      else
		keyboard_out(key_to_pkey[intuition_msg->Code]);
	    }
	  else
	    keyboard_out(key_to_pkey[intuition_msg->Code]);
	  
	  ReplyMsg((struct Message *)intuition_msg);
	}
      /* Handle window active/inactive */
      if (intuition_msg->Class==IDCMP_ACTIVEWINDOW)
	{
	  screen_update_title();
	}
      /* handle menu selection messages */
      if (intuition_msg->Class == IDCMP_MENUPICK) {
          menuNumber = intuition_msg->Code;
          menuNum = MENUNUM(menuNumber);
          itemNum = ITEMNUM(menuNumber);
          subNum = SUBNUM(menuNumber);
          while(menuNumber != MENUNULL) {
              item = ItemAddress(&menu1,menuNumber); 
            if(menuNum == 0 && itemNum == 0 && subNum == 31)
            {
                done();
            }
            menuNumber = item->NextSelect;
          }
          ReplyMsg((struct Message *)intuition_msg);
      }
    }
}

/**
 * keyboard_clear() - Clear the keyboard buffer
 */
void keyboard_clear(void)
{
}

/**
 * keyboard_out_tty - keyboard output to serial I/O in TTY mode
 */
void keyboard_out_tty(char ch)
{
  io_send_byte(ch);
}
