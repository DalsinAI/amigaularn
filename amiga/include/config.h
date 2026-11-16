/* uLarn's config.h for the Amiga port (upstream's configure makes it on
 * Unix): what libnix on AmigaOS 3.2 has. HAVE_TERMIO_H makes header.h take
 * amiga/include/termio.h, which is empty: amiga/amiga_tty.c stands in for
 * upstream's tty.c and amiga/amiga_term.c for termcap. No HAVE_LIBZ: the
 * saved games are written plain. No HAVE_SETITIMER or HAVE_FTIME: nap.c's
 * ularn_napms is amiga/amiga_tty.c's, with dos.library Delay().
 * Copyright (c) 2026 AmiCradle Team. GPL-2.0, like uLarn. */
#define HAVE_FCNTL_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_TERMIO_H 1
#define HAVE_STRCSPN 1
#define RETSIGTYPE void
#define VOIDSIG 1
#define STDC_HEADERS 1
#define PACKAGE_NAME "Ularn"
#define PACKAGE_VERSION "1.7.0"
