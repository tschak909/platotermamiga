/* Menu definition */
#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <intuition/intuition.h>

   struct IntuiText text1 = 
   { 0, 1, JAM2, 4, 2, NULL, "Quit", NULL };
   
   struct MenuItem item1 =
   { NULL, 0, 0, 48, 12, ITEMTEXT|ITEMENABLED|HIGHCOMP, 0, &text1, &text1, NULL,    NULL, 0 };
   
   struct Menu menu1 = 
   { NULL, 0, 0, 48, 12, MENUENABLED, "File", &item1, 0, 0, 0, 0    };

/* Add our menu to the menustrip */
void menu_init() {
    extern struct Window *myWindow;
    SetMenuStrip(myWindow,&menu1);
}
