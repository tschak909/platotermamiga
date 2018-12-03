/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * terminal.c - Terminal state functions
 */


#include <string.h>
#include "terminal.h"
#include "screen.h"

#define true 1
#define false 0

/**
 * ASCII Features to return in Features
 */
#define ASC_ZFGT        0x01
#define ASC_ZPCKEYS     0x02
#define ASC_ZKERMIT     0x04
#define ASC_ZWINDOW     0x08

/**
 * protocol.c externals
 */
extern CharMem CurMem;
extern padBool TTY;
extern padBool ModeBold;
extern padBool Rotate;
extern padBool Reverse;
extern DispMode CurMode;
extern padBool FlowControl;

/**
 * screen.c externals
 */
extern unsigned char CharWide;
extern unsigned char CharHigh;
extern padPt TTYLoc;

extern unsigned char already_started;

static unsigned char char_data[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static unsigned short bold_char_data[]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
					0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

static unsigned char BTAB[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
const unsigned short DBLORTAB[8]={0x0003,0x000C,0x0030,0x00C0,0x0300,0x0C00,0x3000,0xC000};

static unsigned short u,v;
static unsigned char pix_cnt;
static unsigned char curr_word;

extern unsigned char* fontm23;
extern unsigned short* fontm23_bold;

/**
 * Initialize terminal state
 */
void terminal_init(void)
{
  terminal_set_tty();
}

/**
 * terminal_initial_position()
 * Set terminal initial position after splash screen.
 */
void terminal_initial_position(void)
{
  TTYLoc.x=0;
  TTYLoc.y=240;
}

/**
 * terminal_set_tty(void) - Switch to TTY mode
 */
void terminal_set_tty(void)
{
  if (already_started)
    screen_clear();
  TTY=true;
  ModeBold=padF;
  Rotate=padF;
  Reverse=padF;
  CurMem=M0;
  CurMode=ModeWrite;
  CharWide=8;
  CharHigh=16;
  TTYLoc.x = 0;
  TTYLoc.y = 495;
}

/**
 * terminal_set_plato(void) - Switch to PLATO mode
 */
void terminal_set_plato(void)
{
  TTY=false;
  screen_clear();
  CharWide=8;
  CharHigh=16;
}

/**
 * terminal_get_features(void) - Inquire about terminal ASCII features
 */
unsigned char terminal_get_features(void)
{
  return ASC_ZFGT; /* This terminal can do Fine Grained Touch (FGT) */
}

/**
 * terminal_get_type(void) - Return the appropriate terminal type
 */
unsigned char terminal_get_type(void)
{
  return 12; /* ASCII terminal type */
}

/**
 * terminal_get_subtype(void) - Return the appropriate terminal subtype
 */
unsigned char terminal_get_subtype(void)
{
  return 1; /* ASCII terminal subtype IST-III */
}

/**
 * terminal_get_load_file(void) - Return the appropriate terminal loadfile (should just be 0)
 */
unsigned char terminal_get_load_file(void)
{
  return 0; /* This terminal does not load its resident from the PLATO system. */
}

/**
 * terminal_get_configuration(void) - Return the terminal configuration
 */
unsigned char terminal_get_configuration(void)
{
  return 0x40; /* Touch panel is present. */
}

/**
 * terminal_get_char_address(void) - Return the base address of the character set.
 */
unsigned short terminal_get_char_address(void)
{
  return 0x3000; /* What the? Shouldn't this be 0x3800? */
}

/**
 * terminal_mem_read - Read a byte of program memory.
 * not needed for our terminal, but must
 * be decoded.
 */
padByte terminal_mem_read(padWord addr)
{
  return (0xFF);
}

/**
 * terminal_mem_load - Write a byte to non-character memory.
 * not needed for our terminal, but must be decoded.
 */
void terminal_mem_load(padWord addr, padWord value)
{
  /* Not Implemented */
}

/**
 * Mode5, 6, and 7 are basically stubbed.
 */
void terminal_mode_5(padWord value)
{
}

void terminal_mode_6(padWord value)
{
}

void terminal_mode_7(padWord value)
{
}

/**
 * terminal_ext_allow - External Input allowed. Not implemented.
 */
void terminal_ext_allow(padBool allow)
{
  /* Not Implemented */
}

/**
 * terminal_set_ext_in - Set which device to get input from.
 * Not implemented
 */
void terminal_set_ext_in(padWord device)
{
}

/**
 * terminal_set_ext_out - Set which device to send external data to.
 * Not implemented
 */
void terminal_set_ext_out(padWord device)
{
}

/**
 * terminal_ext_in - get an external input from selected device.
 * Not implemented.
 */
padByte terminal_ext_in(void)
{
  return 0;
}

/**
 * terminal_ext_out - Send an external output to selected device
 * Not implemented.
 */
void terminal_ext_out(padByte value)
{
}

void terminal_char_load(padWord charNum, charData theChar)
{
  memset(char_data,0,sizeof(char_data));

  for (curr_word=0;curr_word<8;curr_word++)
    {
      for (u=16; u-->0; )
	{
	  if (theChar[curr_word] & 1<<u)
	    {
	      char_data[u^0x0F&0x0F]|=BTAB[curr_word];
	    }
	}
    }

  for (curr_word=0;curr_word<8;curr_word++)
    {
      for (u=16; u-->0; )
	{
	  if (theChar[curr_word] & 1<<u)
	    {
	      bold_char_data[u^0x0F&0x0F]|=DBLORTAB[curr_word];
	      bold_char_data[(u+1)^0x0F&0x0fF]|=DBLORTAB[curr_word];
	    }
	}      
    }
  
  
  fontm23[(charNum)+0]=char_data[0];
  fontm23[(charNum)+128]=char_data[1];
  fontm23[(charNum)+256]=char_data[2]|char_data[3];
  fontm23[(charNum)+384]=char_data[4];
  fontm23[(charNum)+512]=char_data[5];
  fontm23[(charNum)+640]=char_data[6]|char_data[7];
  fontm23[(charNum)+768]=char_data[8];
  fontm23[(charNum)+896]=char_data[9];
  fontm23[(charNum)+1024]=char_data[10]|char_data[11];
  fontm23[(charNum)+1152]=char_data[12];
  fontm23[(charNum)+1280]=char_data[13];
  fontm23[(charNum)+1408]=char_data[14]|char_data[15];

  // Put bold_char_data into bold character set.
  fontm23_bold[(charNum)+0]=bold_char_data[0];
  fontm23_bold[(charNum)+128]=bold_char_data[1];
  fontm23_bold[(charNum)+256]=bold_char_data[2];
  fontm23_bold[(charNum)+384]=bold_char_data[3];
  fontm23_bold[(charNum)+512]=bold_char_data[4]|bold_char_data[5];
  fontm23_bold[(charNum)+640]=bold_char_data[6]|bold_char_data[7];
  fontm23_bold[(charNum)+768]=bold_char_data[8];
  fontm23_bold[(charNum)+896]=bold_char_data[9];
  fontm23_bold[(charNum)+1024]=bold_char_data[10];
  fontm23_bold[(charNum)+1152]=bold_char_data[11];
  fontm23_bold[(charNum)+1280]=bold_char_data[12]|bold_char_data[13];
  fontm23_bold[(charNum)+1408]=bold_char_data[14]|bold_char_data[15];
  fontm23_bold[(charNum)+1536]=bold_char_data[16];
  fontm23_bold[(charNum)+1664]=bold_char_data[17];
  fontm23_bold[(charNum)+1792]=bold_char_data[18];
  fontm23_bold[(charNum)+1920]=bold_char_data[19];
  fontm23_bold[(charNum)+2048]=bold_char_data[20]|bold_char_data[21];
  fontm23_bold[(charNum)+2176]=bold_char_data[22]|bold_char_data[23];
  fontm23_bold[(charNum)+2304]=bold_char_data[24];
  fontm23_bold[(charNum)+2432]=bold_char_data[25];
  fontm23_bold[(charNum)+2560]=bold_char_data[26];
  fontm23_bold[(charNum)+2688]=bold_char_data[27];
  fontm23_bold[(charNum)+2816]=bold_char_data[28]|bold_char_data[29];
  fontm23_bold[(charNum)+2944]=bold_char_data[30]|bold_char_data[31];  
}

/**
 * terminal_status - Indicate whether we are in TTY or PLATO
 * mode for the status display.
 */
char* terminal_status(void)
{
  if (TTY)
    return "TTY  ";
  else
    return "PLATO";
}
