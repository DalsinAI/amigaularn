/* amiga/amiga_tty.c: the Amiga port's terminal for uLarn.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#ifndef AMIGA_TTY_H
#define AMIGA_TTY_H
int amiga_open_window(void);
int amiga_getkey(void);
char *amiga_player_name(void);
void amiga_flushkeys(void);
#endif
