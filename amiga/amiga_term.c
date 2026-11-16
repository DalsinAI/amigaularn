/* termcap for the Amiga port of uLarn: the strings of the Amiga console
 * (console.device, as CON: gives it), which takes the ANSI control
 * sequences (CSI, $9B). uLarn asks termcap for its terminal's strings
 * (io.c init_term) and writes them with tputs and tgoto; here they are
 * fixed, whatever TERM says.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#include <stdio.h>
#include <string.h>

#include "amiga_term.h"

short ospeed;

static const struct { const char *id, *s; } caps[] = {
	{ "cl", "\233H\233J" },	/* clear the screen, cursor home */
	{ "ce", "\233K" },	/* clear to the end of the line */
	{ "cd", "\233J" },	/* clear to the end of the screen */
	{ "cm", "\233%d;%dH" },	/* cursor to a row and column (tgoto) */
	{ "so", "\2337m" },	/* standout: inverse */
	{ "se", "\2330m" },	/* standout off */
	{ "al", "\233L" },	/* insert a line */
	{ "dl", "\233M" },	/* delete a line */
	{ "up", "\233A" },	/* cursor up */
	{ "ho", "\233H" },	/* cursor home */
	{ "le", "\b" },		/* cursor left */
};

int tgetent(char *bp, const char *name)
{
	(void)bp; (void)name;
	return 1;
}

int tgetflag(const char *id)
{
	return strcmp(id, "bs") == 0; /* the console backspaces */
}

int tgetnum(const char *id)
{
	if (strcmp(id, "co") == 0) return 80;
	if (strcmp(id, "li") == 0) return 24;
	return -1;
}

char *tgetstr(const char *id, char **area)
{
	unsigned i;

	for (i = 0; i < sizeof caps / sizeof caps[0]; i++) {
		if (strcmp(caps[i].id, id) == 0) {
			char *s = *area;
			strcpy(s, caps[i].s);
			*area += strlen(s) + 1;
			return s;
		}
	}
	return NULL;
}

/* only the cursor motion string is ever given: rows and columns count
 * from 0 here and from 1 in the console */
char *tgoto(const char *cm, int col, int row)
{
	static char buf[32];

	(void)cm;
	snprintf(buf, sizeof buf, "\233%d;%dH", row + 1, col + 1);
	return buf;
}

int tputs(const char *s, int affcnt, int (*putc_fn)(int))
{
	(void)affcnt;
	if (s == NULL) return -1;
	while (*s) putc_fn((unsigned char)*s++);
	return 0;
}
