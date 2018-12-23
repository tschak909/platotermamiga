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
#include "help.h"
#include "touch.h"
#include "palette_debug.h"
#include "requester_devices.h"
#include "requester_hang_up.h"
#include "prefs.h"

struct IntuiMessage* intuition_msg;
extern struct Window *myWindow;
unsigned char shift_state=0;
extern void done(void);
extern unsigned short scaletx[];
extern unsigned short scalety[];
extern unsigned char device_requester_is_active;
extern struct Gadget devicesRequesterGadget_apply;

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
      if (intuition_msg->Class == IDCMP_CLOSEWINDOW) {
          struct Window *tmp;
          extern struct Window *windowHelpKeys;
	  extern struct Window *windowPaletteDebug;
          tmp = intuition_msg->IDCMPWindow;
          ReplyMsg((struct Message *) intuition_msg);
          if(tmp == windowHelpKeys) /* Oh, hey it's our help window, close it */
	    help_done();
	  else if (tmp==windowPaletteDebug)
	    palette_debug_done();
          continue; /* Once we've replied we can't do anything else with the struct */
      }
      else if (intuition_msg->Class == MOUSEBUTTONS)
	{
	  if ((intuition_msg->MouseX + 64) < 640 &&
	      (intuition_msg->MouseX + 64) > 64  &&
	      (intuition_msg->MouseY) < 400      &&
	      (intuition_msg->MouseY) > 0)
	    {
	      padPt Coord={scaletx[intuition_msg->MouseX-64],scalety[intuition_msg->MouseY]};
	      Touch(&Coord);
	    }
	}
      else if (intuition_msg->Class == RAWKEY)
	{
	  if (intuition_msg->Code==0x5F && intuition_msg->Qualifier & IEQUALIFIER_LSHIFT) // LSHIFT-Help key
	    keyboard_out(PKEY_HELP1);
	  else if (intuition_msg->Code==0x5F && intuition_msg->Qualifier & IEQUALIFIER_RSHIFT) // LSHIFT-Help key.
	    keyboard_out(PKEY_HELP1);
	  else if (intuition_msg->Code==0x5F)
	    keyboard_out(PKEY_HELP);
	}
      else if (intuition_msg->Class == VANILLAKEY)
	{
	  if (intuition_msg->Qualifier & IEQUALIFIER_RCOMMAND)
	    {
	      if (intuition_msg->Code=='x')
		{
		  done();
		}
	      else if (intuition_msg->Code=='k')
		{
		  help_keys_show();
		}
	      else if (intuition_msg->Code=='l')
		{
		  palette_debug_show();
		  palette_debug_update();
		}
	      else if (intuition_msg->Code=='h')
		{
		  requester_hang_up_do();
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
	      else if ((intuition_msg->Qualifier & IEQUALIFIER_LALT) ||
		       (intuition_msg->Qualifier & IEQUALIFIER_RALT))
		{
		  if (intuition_msg->Qualifier & IEQUALIFIER_LSHIFT)
		    {
		      keyboard_out(PKEY_ACCESS);
		      keyboard_out(key_to_pkey[intuition_msg->Code]);
		    }
		  else if (intuition_msg->Qualifier & IEQUALIFIER_RSHIFT)
		    {
		      keyboard_out(PKEY_ACCESS);
		      keyboard_out(key_to_pkey[intuition_msg->Code]);
		    }
		  else
		    {
		      keyboard_out(PKEY_MICRO);
		      keyboard_out(key_to_pkey[intuition_msg->Code]);
		    }
		}
	      else
		keyboard_out(key_to_pkey[intuition_msg->Code]);
	    }
	  else
	    keyboard_out(key_to_pkey[intuition_msg->Code]);
	  
	  ReplyMsg((struct Message *)intuition_msg);
	}
      /* Handle window active/inactive */
      else if (intuition_msg->Class==IDCMP_ACTIVEWINDOW || intuition_msg->Class==IDCMP_INACTIVEWINDOW)
	{
	  screen_update_title();
	}
      /* handle menu selection messages */
      else if (intuition_msg->Class == IDCMP_MENUPICK)
	{
          menuNumber = intuition_msg->Code;
          menuNum = MENUNUM(menuNumber);
          itemNum = ITEMNUM(menuNumber);
          subNum = SUBNUM(menuNumber);
          while(menuNumber != MENUNULL)
	    {
	      item = ItemAddress(&menuTerminal,menuNumber); 
	      if(menuNum == 0 && itemNum == 4 && subNum == 31)
		{
		  done();
		}
	      else if (menuNum==0&&itemNum==3&&subNum==31)
		{
		  screen_about();
		}
	      else if (menuNum==0&&itemNum==1&&subNum==31)
		{
		  requester_hang_up_do();
		}
	      else if (menuNum==0&&itemNum==0&&subNum==31)
		{
		  if (TTY==padT)
		    InitPLATO();
		  else
		    InitTTY();
		}
	      else if (menuNum==3&&itemNum==0&&subNum==31)
		{
		  help_keys_show();
		}
	      else if (menuNum==3&&itemNum==1&&subNum==31)
		{
		  palette_debug_show();
		  palette_debug_update();
		}
	      else if (menuNum==2&&itemNum==0&&subNum==31)
		{
		  requester_devices_run();
		}
	      else if (menuNum==2&&itemNum==3&&subNum==31)
		{
		  io_set_baud(300);
		}
	      else if (menuNum==2&&itemNum==4&&subNum==31)
		{
		  io_set_baud(1200);
		}
	      else if (menuNum==2&&itemNum==5&&subNum==31)
		{
		  io_set_baud(2400);
		}
	      else if (menuNum==2&&itemNum==6&&subNum==31)
		{
		  io_set_baud(4800);
		}
	      else if (menuNum==2&&itemNum==7&&subNum==31)
		{
		  io_set_baud(9600);
		}
	      else if (menuNum==2&&itemNum==8&&subNum==31)
		{
		  io_set_baud(19200);
		}
	      else if (menuNum==2&&itemNum==9&&subNum==31)
		{
		  io_set_baud(38400);
		}
	      else if (menuNum==2&&itemNum==10&&subNum==31)
		{
		  io_set_baud(57600);
		}
	      else if (menuNum==2&&itemNum==11&&subNum==31)
		{
		  io_set_baud(115200);
		}
	      else if (menuNum==2&&itemNum==13&&subNum==31)
		{
		  io_toggle_rtscts();
		}
	      menuNumber = item->NextSelect;
	    }
	}
      else if (device_requester_is_active==1)
	{
	  // TODO: come back here and add code to focus unit gadget if enter pressed in device menu.
	  if (intuition_msg->Class==GADGETUP && intuition_msg->IAddress == (APTR) &devicesRequesterGadget_apply)
	    {
	      requester_devices_done();
	      prefs_save();
	      io_done();
	      io_init();
	    }
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
