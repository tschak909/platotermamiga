/**
 * PLATOTermAmiga - A PLATO Terminal for the Commodore Amiga
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * io.h - Input/output functions (serial/ethernet)
 */

#include "io.h"

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <devices/serial.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>

struct MsgPort  *SerialMP;          /* Define storage for one pointer */
struct IOExtSer *SerialIO;         /* Define storage for one pointer */

#define READ_BUFFER_SIZE 2048
char SerialReadBuffer[READ_BUFFER_SIZE]; /* Reserve SIZE bytes storage */

struct IOExtSer *SerialWriteIO = 0;
struct MsgPort  *SerialWriteMP = 0;

ULONG Temp;
ULONG WaitMask;



/**
 * io_init() - Set-up the I/O
 */
void io_init(void)
{
}

/**
 * io_init_funcptrs() - Set up I/O function pointers
 */
void io_init_funcptrs(void)
{
}

/**
 * io_open() - Open the device
 */
void io_open(void)
{
}

/**
 * io_send_byte(b) - Send specified byte out
 */
void io_send_byte(unsigned char b)
{
}

/**
 * io_main() - The IO main loop
 */
void io_main(void)
{
}

/**
 * io_recv_serial() - Receive and interpret serial data.
 */
void io_recv_serial(void)
{
}


/**
 * io_done() - Called to close I/O
 */
void io_done(void)
{
}
