/* simple sound library based on the scales program on fred fish disk 6 */

void strike(int note, int voice); /* sound note  0-83 (100 for silence ) and voice 0-3 */ 
void restall();                   /* silence all channels */
void cchord();                    /* C chord using all 4 voices */
void sound_init();                /* setup sound device and wave table */
void sound_done();                /* close sound device and free memory */
void sound_beeper();              /* Sound the beeper */

/* See the document sound.txt and src/sound.c for more details. 
 * The following is an example of what can be done (the original sequence of calls
 * in the scale program.
 * 
 * you must first call sound_init() and then sound_done() before closing the program.
 * more of the internals of sound.c will be exposed as we need them.
 */

//  /* simple scale
//    */
//   for (i = 0; i < 24; ++i)
//      {
//      strike(i, 0);
//      Delay(10L);
//      }
// 
//   /* crossed scales
//    */
//   for (j = 48; i < 48; ++i, --j)
//      {
//      strike(i, 0);
//      strike(j, 1);
//      Delay(10L);
//      }
// 
//   /* crossed scales with lower in sync
//    */
//   for (j = 48; i < 72; ++i, --j)
//      {
//      strike(i, 0);
//      strike(i - 48, 1);
//      strike(j, 2);
//      Delay(10L);
//      }
// 
//   /* slow c chord...
//    */
//   strike(36, 0);
//   strike(100, 1);
//   strike(100, 2);
//   Delay(60L);
//   strike(40, 1);
//   Delay(60L);
//   strike(43, 2);
//   Delay(120L);
// 
//   /* ...and into a minor with the fourth voice
//    */
//   strike(46, 3);
//  Delay(200L);
// 
//   /* and two quick c chords to finish it up
//    */
//   restall();
//   Delay(20L);
//   cchord();
//   Delay(20L);
//   restall();
//   Delay(1L);
//   cchord();
//   Delay(200L);
//   restall();
//
