void menu_init();
extern struct Menu menuTerminal;

/**
 * toggle PLATO mode checkbox
 */
void menu_toggle_plato_mode(unsigned char toggle);

/**
 * update baud rate in menu
 */
void menu_update_baud_rate(long baud_rate);

/* Remove our menu when done */
void menu_done(void);
