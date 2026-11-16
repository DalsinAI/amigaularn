/* The Amiga console's keys as uLarn reads them (amiga/amiga_keys.h). In
 * raw mode (SetMode 1) the console gives Return as CR, which a Unix tty
 * gives uLarn as NL; the cursor keys as CSI ($9B, or Esc [) and A, B, C or
 * D; the function keys, Help and window events as longer CSI sequences
 * ending in a letter, '~' or '|'. No AmigaOS calls here, so
 * tests/host/test_keys.c runs it on the build machine.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#include <stdio.h>

#include "amiga_keys.h"

#define CSI 0x9b
#define ESC 0x1b

int amiga_key_decode(int (*next)(void), void (*unget)(int))
{
	int c = next(), n;

	if (c == '\r') {
		return '\n';
	}
	if (c != CSI && c != ESC) {
		return c;
	}
	if (c == ESC) {
		n = next();
		if (n != '[') { /* Esc alone, or Esc and a key */
			if (n != EOF) unget(n);
			return ESC;
		}
	}
	n = next();
	switch (n) {
	case 'A': return 'k';
	case 'B': return 'j';
	case 'C': return 'l';
	case 'D': return 'h';
	}
	/* to the end of the sequence */
	while (n != EOF && !((n >= '@' && n <= '~') || n == '|')) {
		n = next();
	}
	return n == EOF ? EOF : 0;
}
