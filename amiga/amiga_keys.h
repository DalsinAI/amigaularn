/* amiga/amiga_keys.c: the Amiga console's keys as uLarn reads them.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#ifndef AMIGA_KEYS_H
#define AMIGA_KEYS_H
/* next() gives the console's next byte (EOF at the end), unget() puts one
 * back. Returns the key: Return (CR) as NL, the cursor keys (CSI A..D) as
 * k, j, l and h, Esc as Esc, any other CSI sequence read to its end and
 * given as 0, EOF at the end of input. */
int amiga_key_decode(int (*next)(void), void (*unget)(int));
#endif
