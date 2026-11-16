/* amiga/amiga_term.c: termcap's calls, as uLarn makes them.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#ifndef AMIGA_TERM_H
#define AMIGA_TERM_H
int tgetent(char *bp, const char *name);
int tgetflag(const char *id);
int tgetnum(const char *id);
char *tgetstr(const char *id, char **area);
char *tgoto(const char *cm, int col, int row);
int tputs(const char *s, int affcnt, int (*putc_fn)(int));
#endif
