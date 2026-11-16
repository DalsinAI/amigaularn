/* Host test of amiga/amiga_keys.c and amiga/amiga_term.c: the console's
 * keys as uLarn reads them, and the strings uLarn writes for its screen.
 * Built and run by tests/host/test_amiga.sh.
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#include <stdio.h>
#include <string.h>

#include "amiga_keys.h"
#include "amiga_term.h"

static const unsigned char *in;
static size_t in_len, in_pos;
static int fails, checks;

static int next(void) { return in_pos < in_len ? in[in_pos++] : EOF; }
static void unget(int c) { (void)c; in_pos--; }

static void keys(const char *what, const char *bytes, size_t len, const int *want, int n)
{
	int i;

	in = (const unsigned char *)bytes; in_len = len; in_pos = 0;
	for (i = 0; i < n; i++) {
		int got = amiga_key_decode(next, unget);
		checks++;
		if (got != want[i]) {
			printf("FAIL keys %s: key %d is %d, not %d\n", what, i, got, want[i]);
			fails++;
			return;
		}
	}
}

static char out[256];
static size_t out_len;
static int put(int c) { out[out_len++] = (char)c; return c; }

static void str(const char *what, const char *got, const char *want)
{
	checks++;
	if (got == NULL || strcmp(got, want) != 0) {
		printf("FAIL term %s\n", what);
		fails++;
	}
}

int main(void)
{
	static const int k1[] = { 'h', 'j', '\n', 'k', 'l', 'q', EOF };
	static const int k2[] = { 'k', 'j', 'l', 'h', EOF };
	static const int k3[] = { 0, 'i', 0, 0, 'x', EOF };
	static const int k4[] = { 0x1b, 'q', 0x1b, EOF };
	char area[256], *ap = area, *s;

	keys("plain", "hj\rklq", 6, k1, 7);
	keys("CSI cursor", "\233A\233B\233C\233D", 8, k2, 5);
	keys("Esc [ cursor", "\033[A\033[B\033[C\033[D", 12, k2, 5);
	/* F1 (CSI 0~), a window event (resized: CSI 12;0;0;0;0;0;0;0|), Help (CSI ?~) */
	keys("CSI dropped", "\2330~i\23312;0;0;0;0;0;0;0|\233?~x", 26, k3, 6);
	/* the close gadget (CSI 11;...|) ends the input, keys after it or not;
	 * class 1 (a raw key event) is not it */
	keys("close gadget", "i\23311;0;0;0;0;0;0;0|x", 20, (const int[]){ 'i', EOF }, 2);
	keys("class 1 event", "\2331;0;0;0;0;0;0;0|x", 18, (const int[]){ 0, 'x', EOF }, 3);
	keys("Esc", "\033q\033", 3, k4, 4);
	keys("CSI cut short", "\2331", 2, (const int[]){ EOF }, 1);

	checks++;
	if (tgetent(area, "dumb") != 1) { printf("FAIL tgetent\n"); fails++; }
	str("cl", tgetstr("cl", &ap), "\233H\233J");
	str("ce", tgetstr("ce", &ap), "\233K");
	str("cd", tgetstr("cd", &ap), "\233J");
	str("so", tgetstr("so", &ap), "\2337m");
	str("se", tgetstr("se", &ap), "\2330m");
	str("al", tgetstr("al", &ap), "\233L");
	str("dl", tgetstr("dl", &ap), "\233M");
	s = tgetstr("cm", &ap);
	checks++;
	if (s == NULL) { printf("FAIL cm\n"); fails++; }
	checks++;
	if (tgetstr("xx", &ap) != NULL) { printf("FAIL unknown capability\n"); fails++; }
	/* io.c's lflush: column, then row, both from 0 */
	str("tgoto 0,0", tgoto(s, 0, 0), "\2331;1H");
	str("tgoto 66,17", tgoto(s, 66, 17), "\23318;67H");
	str("tgoto 0,23", tgoto(s, 0, 23), "\23324;1H");
	out_len = 0;
	tputs(tgoto(s, 9, 4), 0, put);
	out[out_len] = 0;
	str("tputs", out, "\2335;10H");
	checks++;
	if (!tgetflag("bs") || tgetflag("os")) { printf("FAIL tgetflag\n"); fails++; }
	printf("amiga keys and termcap: %d checks, %d failed\n", checks, fails);
	return fails != 0;
}
