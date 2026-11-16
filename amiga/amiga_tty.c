/* The terminal of the Amiga port of uLarn, in place of upstream's tty.c,
 * and the rest of what the game asks of Unix that AmigaOS answers
 * otherwise.
 *
 * The window: uLarn draws on a terminal of 80 columns by 24 lines. The
 * game always opens its own console window (CON:) that size in the
 * console's font (the screen's default font: graphics.library's
 * DefaultFont), from a Shell as from Workbench, makes it the program's
 * console and opens stdin, stdout and stderr on it ("*"). Its close gadget
 * ends the input: uLarn then saves the game, as its S command does
 * (amiga_getkey).
 *
 * The keys: upstream's tty.c sets cbreak with no echo; here the console is
 * put in raw mode (dos.library SetMode 1), which reads a key at a time and
 * does not echo. Return comes as CR, which a Unix tty gives as NL: it is
 * given as NL. The cursor keys come as CSI A..D and are given as uLarn's
 * k, j, l and h; any other CSI sequence (function keys, the Help key,
 * window events) is read to its end and dropped.
 *
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dos/dos.h>
#include <dos/dosextens.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "header.h"
#include "player.h"
#include "extern.h"
#include "amiga_tty.h"
#include "amiga_keys.h"

extern struct GfxBase *GfxBase;

/* Version shows it; __attribute__((used)) keeps it past --gc-sections */
static const char version[] __attribute__((used)) =
    "$VER: uLarn 1.7.0 (16.11.2026)";

static BPTR console;
static int raw;

static void console_mode(int mode)
{
	if (console && raw != mode) {
		SetMode(console, mode);
		raw = mode;
	}
}

static void console_end(void)
{
	if (console) {
		fputs("\233 p", stdout); /* the cursor shows again */
		fflush(stdout);
		console_mode(0);
		/* stdin, stdout and stderr ("*") close after this; the window
		 * goes with the last of them */
		Close(console);
		console = 0;
	}
}

int amiga_open_window(void)
{
	struct Process *me = (struct Process *)FindTask(NULL);
	struct TextFont *tf = GfxBase ? GfxBase->DefaultFont : NULL;
	int fw = tf ? tf->tf_XSize : 8, fh = tf ? tf->tf_YSize : 8;
	char spec[96];

	/* 80 x 24 characters, the borders and the title bar */
	snprintf(spec, sizeof spec, "CON:0/16/%d/%d/uLarn/CLOSE",
	         80 * fw + 24, 24 * fh + 2 * fh + 20);
	console = Open((CONST_STRPTR)spec, MODE_NEWFILE);
	if (!console) {
		return 0;
	}
	atexit(console_end);
	me->pr_ConsoleTask = (APTR)((struct FileHandle *)BADDR(console))->fh_Type;
	if (!freopen("*", "r", stdin) || !freopen("*", "w", stdout) ||
	    !freopen("*", "w", stderr)) {
		return 0;
	}
	setvbuf(stdin, NULL, _IONBF, 0); /* a key is read as it comes */
	/* in raw mode the close gadget sends nothing unless its raw event
	 * (class 11) is asked for: it then comes as CSI 11;...| */
	fputs("\23311{", stdout);
	fflush(stdout);
	return 1;
}

/* upstream's tty.c */
int setctty(void) { return 0; }
int gettty(void) { setuptty(); return 0; }
int settty(void) { console_mode(0); return 0; }
int setuptty(void) { console_mode(1); return 0; }
int scbr(void) { console_mode(1); return 0; }
int sncbr(void) { console_mode(0); return 0; }

int setupvt100(void)
{
	clear();
	setscroll();
	gettty();
	return 0;
}

int clearvt100(void)
{
	resetscroll();
	settty();
	return 0;
}

/* nap.c's: dos.library counts 50 ticks a second */
void ularn_napms(int ms)
{
	if (ms > 0) {
		Delay((ms + 19) / 20);
	}
}

/* a key, as a Unix tty in cbreak mode gives it (amiga/amiga_keys.c);
 * EOF at the end of input */
static int key_next(void) { return getc(stdin); }
static void key_unget(int c) { ungetc(c, stdin); }

int amiga_getkey(void)
{
	return amiga_key_decode(key_next, key_unget);
}

/* What libnix has not got: the player is named by the USER variable, or
 * "Adventurer"; there are no other processes to fork or wait for (uLarn
 * mails no bill; libnix's fork() fails). */
char *amiga_player_name(void)
{
	static char name[40];
	char *u = getenv("USER");

	strncpy(name, u && *u ? u : "Adventurer", sizeof name - 1);
	return name;
}

int wait(int *status) { (void)status; return -1; }

/* tok.c's flushall: the keys typed ahead are dropped */
void amiga_flushkeys(void)
{
	BPTR in = console ? console : Input();

	while (in && WaitForChar(in, 0)) {
		if (getc(stdin) == EOF) {
			break;
		}
	}
}
