/* Sound routines for Amiga platoterm. This is a re-use of the SCALES program by
 * Steven A. Bennett from Fred Fish disk 6 that I hope to evolve over time. 
 *
 * Partial original comment below */
/* SCALES
 *
 *   by Steven A. Bennett
 *
 *   This program demonstrates the use of the Audio functions
 * in the "ROM" to produce four voice sound.  It uses a simple
 * waveform (sawtooth) with no amplitude control (ie, envelope)
 * or frequency variation (ie, vibrato), but these can easily be
 * implemented through the use of setpv(), below.
 * This program may be freely distributed.
 *
 */
#include <proto/exec.h> 
#include <proto/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/audio.h>
#include <exec/exec.h>
#include <clib/alib_protos.h>
#include <stdio.h>
#include <stdlib.h>
#include "sound.h"
#undef NULL
#define NULL 0L

#define  PRIORITY          10L    /* Priority for Audio Channel usage */
#define  NBR_IOA_STRUCTS   10L    /* Number of IOAudio structures used */
#define  PV_IOA_STRUCT     0L     /* index to ioapv struct */
#define  FIN_IOA_STRUCT    9L     /* index to finishioa struct */
#define  BIG_WAVE          256L   /* size of biggest waveform */
#define  NBR_WAVES         7L     /* number of waves per instrument */
#define  WAVES_TOTAL       1024L  /* alloc size for instrument's waves */
#define  YES               1L
#define  NO                0L
 
/* function protos */
void FinishSound(int finishcode); /* Internal use only, keep out of sound.h */

UBYTE aMap[] = { 0x0f };                  /* allocate four channels */
long  voiceMap[] = { 1, 2, 4, 8 };
struct IOAudio *ioa, *finishioa, *ioapv;
struct IOAudio *ioainuse[4];
struct IOAudio *freeioa[4];
long unitno = 1;
int error;
int waiting[4] = { NO, NO, NO, NO };
int woffsets[] =
   { 0, 256, 384, 448, 480, 496, 504, 508, 510 };
int wlen[] =
   { 256, 128, 64, 32, 16, 8, 4, 2, 1 };
int perval[] =
   { 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226, 214 };
BYTE *wptr;
BYTE *owptr[4] = { NULL, NULL, NULL, NULL };
 
char *portstring[] = {
   "Audio one",
   "Audio two",
   "Audio three",
   "Audio four",
   "Audio five",
   "Audio six",
   "Audio seven",
   "Audio eight" };              /* names for the CreatePorts */
 
/* InitIOA()
 *   This function initializes all IOAudio structures used by this
 * program.
 *
 * NOTES
 *   This version sets up enough IOAudio structures for four voices, plus
 * a ADCMD_FINISH and an ADCMD_PERVOL, which are both synchronous.
 * The ioainuse array is assumed to be the IOAudio structure which is
 * being used by the current CMD_WRITE command for each voice.  The freeioa
 * array is there so that no <click> is made when switching CMD_WRITEs.
 * (ie., for speed)  Since the pointers are swapped when the switch occurs,
 * one can always make the same assumption.  Note that ALL of the
 * asynchronous (ie., ioainuse and freeioa) structures ought to have a
 * unique ReplyPort (or so I think).  The IOAudio structure used to
 * Open the Device must also have a ReplyPort, but it may not have to be
 * unique.  I am taking no chances, however.
 */
 
void InitIOA()
   {
   int i;
 
   /* alloc the IOAudio structures
    */
   ioa = (struct IOAudio *)AllocMem((NBR_IOA_STRUCTS * (long)sizeof(*ioa)),
      MEMF_PUBLIC | MEMF_CLEAR);
   if (ioa == NULL)
      FinishSound(1);
 
   /* set the various IOAudio structure pointers
    */
   for (i = 0; i < 4; ++i)
      {
      ioainuse[i] = &ioa[i + 1];
      freeioa[i] = &ioa[i + 5];
      }
   ioapv = &ioa[PV_IOA_STRUCT];
   finishioa = &ioa[FIN_IOA_STRUCT];
 
   /* Open the Audio Device.  This requires a ReplyPort, so we'll
    * make it now, and reuse it later as part of ioapv, although it
    * doesn't need one.  aMap[] is an array containing sets of
    * bitmaps which allow various combinations of voices to be allocated.
    * since we want all four voices, there is only one combination which
    * will serve.
    */
   ioa->ioa_Request.io_Message.mn_Node.ln_Pri = PRIORITY;
   ioa->ioa_Request.io_Message.mn_ReplyPort =
      CreatePort("Audio zero", 0L);
   if (ioa->ioa_Request.io_Message.mn_ReplyPort == NULL)
      FinishSound(2);
   ioa->ioa_Data = aMap;
   ioa->ioa_Length = (long)sizeof(aMap);
   error = OpenDevice(AUDIONAME, 0L,(struct IORequest *) ioa, 0L);
   if (error)
      FinishSound(3);
 
   /* setup the finishioa and ioapv structs.  The IOF_QUICK flag
    * makes them synchronous in all cases.
    */
   *finishioa = *ioa;
   finishioa->ioa_Request.io_Flags = IOF_QUICK;
   ioapv->ioa_Request.io_Flags = IOF_QUICK;
 
   finishioa->ioa_Request.io_Command = ADCMD_FINISH;
   ioapv->ioa_Request.io_Command = ADCMD_PERVOL;
 
   /* setup the ioainuse and freeioa struct arrays.  All eight of
    * them must have unique ReplyPorts, hence the CreatePort()s
    * below.  portstring contains different port names for the
    * eight ports.  (I have no idea as to what the zero in the
    * CreatePort is used for, so don't ask.)
    */
   for (i = 0; i < 4; ++i)
      {
      *freeioa[i] = *ioa;
      *ioainuse[i] = *ioa;
      freeioa[i]->ioa_Request.io_Message.mn_ReplyPort =
         CreatePort(portstring[i], 0L);
      ioainuse[i]->ioa_Request.io_Message.mn_ReplyPort =
         CreatePort(portstring[i + 4], 0L);
      }
   for (i = 0; i < 4; ++i)
      if (freeioa[i]->ioa_Request.io_Message.mn_ReplyPort == NULL ||
         ioainuse[i]->ioa_Request.io_Message.mn_ReplyPort == NULL)
         FinishSound(4);
   }
 
/* FinishSound(finishcode)
 *    int finishcode;
 *
 *    displays an error message, if necessary, based on the value
 * of finishcode, frees all allocated stuff (again based on finishcode)
 *
 * NOTES
 *   This is basically a tidy little "clean up" routine used by all exit
 * routines in this program.  Since all the allocated "stuff" is supposed
 * to be in global variables, freeing it up in one global location is
 * much easier then doing it elsewhere.
 *
 * You should never call this directly from outside of sound.c
 * use sound_done() instead;
 */
 
char *errormsgs[] = {
   "Finished!\n",
   "Cannot allocate memory for IOAudio structures\n",
   "Cannot create ReplyPort for OpenDevice call\n",
   "Cannot open Audio Device\n",
   "Cannot create ReplyPort(s) for remaining IOAudio structures\n",
   "Cannot allocate memory for waveform\n",
   "If you see this during execution, execute the programmer.\n" };
 
void FinishSound(int finishcode)
   {
   int i;
 
   printf(errormsgs[finishcode]);
   switch(finishcode)
      {
   case 0:
      /* free up the WaveNode list
       * (currently, just wptr)
       */
      FreeMem(wptr, WAVES_TOTAL);
 
   case 4:
   case 5:
      /* free up all ReplyPorts save the first.  Since we could be
       * here due to an error in allocating same, we check first.
       * (And yes, Virginia, we do fall through here)
       */
      for (i = 0; i < 4; ++i)
         {
         if (freeioa[i]->ioa_Request.io_Message.mn_ReplyPort)
            DeletePort(freeioa[i]->ioa_Request.io_Message.mn_ReplyPort);
         if (ioainuse[i]->ioa_Request.io_Message.mn_ReplyPort)
            DeletePort(ioainuse[i]->ioa_Request.io_Message.mn_ReplyPort);
         }
 
      /* Close the Audio Device
       */
      CloseDevice((struct IORequest *)ioa);
 
   case 3:
      /* Delete the first ReplyPort
       */
      DeletePort(ioa->ioa_Request.io_Message.mn_ReplyPort);
 
   case 2:
      /* Free the ioa memory
       */
      FreeMem(ioa, (NBR_IOA_STRUCTS * (long)sizeof(*ioa)));
 
      }
   if (finishcode)
      exit(1L);
   exit(0L);
   }
 
/* setwpv()
 *
 *   starts a sound on the channel specified by the global variable
 * unitno.  By swapping between two IOAudio structures, this routine
 * is able to accomplish it's task without a noticeable delay.
 *
 * NOTES
 *   This routine should only be used when a waveform change is
 * required, either due to frequency going out of the range of the
 * existing waveform, or a different waveform has been chosen.  In
 * most cases, setpv() will be sufficient.  Also, don't complain about
 * the usage of io_Unit.  I didn't like it either.
 *
 */
 
void setwpv(char *wf,int len, int  per,int  vol,int voice)
   {
   struct IOAudio *tmpioa;
 
   /* the next three lines are probably unnecessary and can be
    * done instead in InitIOA, but why take chances?
    */
   freeioa[voice]->ioa_Request.io_Command = CMD_WRITE;
   freeioa[voice]->ioa_Request.io_Flags = ADIOF_PERVOL | IOF_QUICK;
   freeioa[voice]->ioa_Cycles = 0;
 
   /* Assign the unit numbers to the (<ahem>) pointer?
    */
   freeioa[voice]->ioa_Request.io_Unit = (struct Unit *)unitno;
   finishioa->ioa_Request.io_Unit = (struct Unit *)unitno;
 
   /* Set the parameters
    */
   freeioa[voice]->ioa_Data = (UBYTE *)wf;
   freeioa[voice]->ioa_Length = len;
   freeioa[voice]->ioa_Period = per;
   freeioa[voice]->ioa_Volume = vol;
 
   /* Terminate the old request, if there is one.  waiting[] is an
    * boolean (well, pseudo-boolean) array stating if there was an
    * old request.  The old CMD_WRITE must be finished, and the reply
    * received, before the new CMD_WRITE will work.  I am not certain
    * why this is necessary.  Originally, I tried a
    *               BeginIO(freeioa[voice]);
    *               BeginIO(finishioa);
    *               WaitIO(ioainuse[voice]);
    * series, but the net effect was that the freeioa request was
    * ignored (without error), and the ioainuse request continued
    * blindly onwards.  I had assumed that it would act as a FIFO queue,
    * but apparently not.
    */
   if (waiting[voice])
      {
      BeginIO((struct IORequest *)finishioa);
      WaitIO((struct IORequest *) ioainuse[voice]);
      waiting[voice] = NO;
      }
 
   /* now start up the new voice
    */
   BeginIO((struct IORequest *)freeioa[voice]);
   error = (int) CheckIO((struct IORequest *)freeioa[voice]);
   if (error)
      {
      printf("Error on CMD_WRITE\n");
      WaitIO((struct IORequest *)freeioa[voice]);
      }
   waiting[voice] = YES;
 
   /* swap the pointers.  That way, the next time we pass through, we
    * will still work.
    */
   tmpioa = ioainuse[voice];
   ioainuse[voice] = freeioa[voice];
   freeioa[voice] = tmpioa;
   }
 
/* setpv(per, vol)
 *   int per, vol;
 *
 *   Changes the period and volume of the currently executing
 * CMD_WRITE on the specified unitno.
 *
 * NOTES
 *   This routine is perfect <ahem> as it is, both for single and
 * multiple voices.  It can be used to change frequency, within limits,
 * change volume, and simulate vibrato and envelope controls.
 * For those of you wondering where I got my period values
 * from, look in the ABASIC manual, page 138.  Or, if you wish, you
 * can calculate it out for yourself using the formula:
 *     period = C/(ns*hz)
 *       - where C is the clock rate (3579545)
 *               ns is the number of samples in a cycle of the wave
 *               hz is the number of cycles per second.
 * Thus, for middle A on the piano (440 hz) with 32 samples in the
 * cycle of the waveform, one would get:
 *     period = 3579545 / (32 * 440)
 *            = 3579545 / 14080
 *            = 254.229 (or pretty close)
 * The period must be rounded to the nearest integer, which can result
 * in a maximum frequency error of about .25%, assuming one uses the
 * octave for frequency between period 226 and period 428.  (This comes
 * out to be less than a twentieth step at the shortest period)
 * period values of less than 127 are illegal, as there aren't enough
 * cycles set aside for audio DMA for anything less.  period values of
 * greater than 500 or so aren't recommended as the anti-aliasing
 * filter isn't of much use then, and actually could cause a possible
 * high pitched overtone, which I'm sure nobody wants.  Thus I am
 * only going to use this to handle a single octave's range.
 * Changes of octave are accomplished by doubling or halving the number
 * of samples in one cycle of the waveform, and must, therefore, call
 * setwpv().
 */
 
void setpv(per, vol)
   int per, vol;
   {
   ioapv->ioa_Period = per;
   ioapv->ioa_Volume = vol;
   ioapv->ioa_Request.io_Unit = (struct Unit *)unitno;
   BeginIO((struct IORequest *)ioapv);
   }
 
/* StopVoices()
 *
 *   Terminates all CMD_WRITE routines in progress, effectively
 * ending all sound.  Volume is set to zero first, just to be tidy.
 *
 */
 
void StopVoices()
   {
   int voice;
 
   for (voice = 0; voice < 4; ++voice)
      {
      if (waiting[voice])
         {
         /* to stop a voice, we first set it's volume to zero
          * (probably unnecessary) and then finish the CMD_WRITE.
          */
         unitno = voiceMap[voice];
         setpv(128, 0);
         finishioa->ioa_Request.io_Unit = (struct Unit *)unitno;
         BeginIO((struct IORequest *)finishioa);
         WaitIO((struct IORequest *)ioainuse[voice]);
         waiting[voice] = NO;
         }
      }
   }
 
/* setwave(wfp)
 *   BYTE wfp;
 *
 *   this routine makes the first 256 bytes (lowest octave) of the
 * sawtooth wave's waveform table.  wfp must have already been
 * allocated and in CHIP MEMORY!!!  This is necessary later for the
 * CMD_WRITE commands to work.
 *   a sawtooth wave is a simple waveform, very easy to use.  It
 * basically looks like this:
 *
 *     /|      /|      /|      /|
 *    / |     / |     / |     / |
 *   /  |    /  |    /  |    /  |
 *  /   |   /   |   /   |   /   |
 *      |  /    |  /    |  /    |  /
 *      | /     | /     | /     | /
 *      |/      |/      |/      |/
 *
 */
 
void setwave(wfp)
   UBYTE *wfp;          /* This is a sneaky way of making a sawtooth */
   {
   int i;
 
   for (i = 0; i < BIG_WAVE; ++i)
      wfp[i] = i;
   }
 
/* xpandwave(wfp)
 *   BYTE *wfp;
 *
 * xpandwave expands a wave with only the base BIG_WAVE specified,
 * into a set of NBR_WAVES waveforms, each for one octave.
 * All of the waveforms are left in contiguous memory after the
 * first wave, in order of decending sizes (256, 128, 64, ...)
 *
 * NOTES
 *   Be forewarned that this function makes two assumptions.  The
 * first is that there is enough memory in the wfp buffer to hold the
 * expansion, and two, that the stuff is in chip mem.  (Actually,
 * makewaves() makes that assumption instead.)  This function ought to
 * work for any BIG_WAVE which is a power of two greater than 2^NBR_WAVES
 *
 */
 
void xpandwave(wfp)
   BYTE *wfp;
   {
   int i, j, rate;
   BYTE *tptr;
 
   rate = 1;
   tptr = wfp + BIG_WAVE;
   for (i = 0; i < NBR_WAVES - 1; ++i)
      {
      rate *= 2;
      for (j = 0; j < BIG_WAVE; j += rate)
         *tptr++ = wfp[j];
      }
   }
 
/* makewaves()
 *
 *   just makes a sawtooth waveform in chip mem and expands it without
 * the pretty list control and file IO stuff.
 *
 */
 
void makewaves()
   {
   /* allocate the memory for the waveform.
    */
   wptr = (BYTE *)AllocMem(WAVES_TOTAL, MEMF_CHIP);
   if (wptr == NULL)
      FinishSound(5);
 
   /* get and expand the waveform
    */
   setwave(wptr);
   xpandwave(wptr);
   }
 
/* strike(note, voice)
 *   int note, voice;
 *
 * This nice little routine takes a note and plays it on the given
 * voice.  At a fixed amplitude.  The note is basically an integer from
 * 0 to 11 (c to b) plus 12 per octave above the first and lowest.  It
 * uses a pointer to the last waveform used by the voice specified to
 * determine if it needs to call setwpv() or just setpv().  The waveform
 * is used by adding an index (woffsets[]) dependant on the octave.
 * the length of the waveform (in wlen[]) is likewise dependant on
 * the octave.  Note that octaves start with zero, not one.
 */
 
void strike(int note,int  voice)
   {
   int per, oct, len;
   BYTE *wfp;
 
   unitno = voiceMap[voice];
   if (note >= 100)           /* play a rest. */
      {
      if (waiting[voice])
         setpv(200, 0);
      return;
      }
   oct = note / 12;
   per = perval[note % 12];
   wfp = wptr + woffsets[oct];
 
   /* if the waveform hasn't changed since the last strike,
    * then only change the period.
    */
   if (wfp == owptr[voice])
      setpv(per, 32);         /* fixed volume */
   else
      {
      setwpv(wfp, wlen[oct], per, 32, voice);
      owptr[voice] = wfp;
      }
   }
/* Open the sound device and do our setup here. */ 
void sound_init() {
    InitIOA();
    makewaves();
}

/* make it easy for external callers to release the
 * sound device and memory 
 */
void sound_done() {
    restall();      /* Silence all voices */
    StopVoices();   /* turn off all channels and abort pending IO */
    FinishSound(0); /* close device, free memory */

}

/* Sound the beeper */
void sound_beeper()
{
   int i;
   strike(60,0);
   Delay(20L);
   restall();
   return; 
}
 
void restall()
   {
   int i;
 
   for (i = 0; i < 4; ++i)
      strike(100, i);
   }
 
void cchord()
   {
   strike(36, 0);
   strike(40, 1);
   strike(43, 2);
   strike(48, 3);
   }
