uLarn for GFOS
==============

A port of uLarn (Ularn), Phil Cordier's roguelike after Noah Morgan's
Larn, kept by Josh Brandt and David Richerby, from
https://github.com/ularn/ularn at commit
ef421842add954c55639656c5973042963c9b258 (version 1.7.0), for GFOS:
AmigaOS 3.2 with GFOpenUp, on a 68040 or 68060.

Your daughter has caught dianthroritis and only a potion from the caves of
Larn can cure her. Choose a class, buy what you can at the Larn stores, go
down through the dungeon and the volcano, and be back before she dies.

Status: built in Cradle's os32-gcc16-open6 stove; the port's console code
passes its host tests; not yet run in a lab.


What you need
-------------

- AmigaOS 3.2 (GFOS) on a 68040 or 68060. The FPU is used when there is
  one; the game needs none (an LC040 is fine).
- About 300 KB of disk and 400 KB of memory. No libraries.


How to play
-----------

Run Install-uLarn (or copy the uLarn drawer where you want it), then
double-click uLarn, or from a Shell:

    uLarn [-n] [-d difficulty] [-r] [-s] [-i] [-h]

The game opens its own window, 80 by 24 characters in the screen's font,
from Workbench and from a Shell. -n skips the welcome, -d sets the
difficulty, -r restores the last checkpoint, -s and -i show the
scoreboard, -h lists the options.

Move with h j k l y u b n (or the cursor keys), ? lists the commands, S
saves and quits; the next start goes on from the save. Closing the window
saves too, as S does. The player's name is the USER variable (SetEnv
USER yourname) or "Adventurer" until the game asks for one.

The saved game, the scoreboard (Uscore) and your options file (.Ularnopts,
as upstream's README describes: name, female, bold-off, no-introduction,
monster names) are in the drawer's User drawer, which Install-uLarn and
GFOpenUp keep when they update the game. The help, the cave levels and the
fortunes are in Data.

Not on the Amiga: the ! shell escape (there is no Unix shell to escape to),
the background checkpoint every 400 moves (enable-checkpointing is
ignored: save with S) and the bill uLarn mails when you win.


Building
--------

    make -f amiga/Makefile.amigaos -j4 UL=/path/to/ularn STOVE=/path/to/os32-gcc16-open6

with UL upstream at the locked commit and amiga/patches applied, or, in
Cradle, stove_build with the os32-gcc16-open6 stove and target package
(package/build-amiga.sh), which fetches and patches upstream, runs the host
tests, packs uLarn-VERSION-68040.lha and makes the GFOpenUp part
Parts/uLarn (GFOpenUpTool INSTALL Parts/uLarn puts the game in
SYS:Games/uLarn). Upstream is fetched at the commit package/ularn.lock
names, not kept in this repository.


What was changed
----------------

- amiga/patches/0001-amigaos-console-and-paths.patch: main.c opens the
  window, names the player and puts the saves, the options and the
  scoreboard in PROGDIR:User; io.c reads the keys through
  amiga/amiga_keys.c, and the window's end saves the game; nap.c's pause
  is dos.library Delay.
- amiga/patches/0002-amigaos-keys.patch: tok.c's command loop reads the
  keys as io.c does, drops the typed-ahead keys from the console, and has
  no shell escape or background checkpoint.
- amiga/amiga_tty.c: in place of upstream's tty.c: the window, raw mode
  (SetMode) where upstream sets cbreak and no echo, and what libnix lacks.
- amiga/amiga_term.c: termcap, answered with the Amiga console's own
  control sequences (CSI): clear, clear to the end of the line and of the
  screen, cursor motion, inverse, insert and delete line.
- amiga/amiga_keys.c: Return as NL, the cursor keys as hjkl, other CSI
  sequences (function keys, Help, window events) dropped.
- amiga/include/: config.h (what upstream's configure would write for
  libnix), and empty termio.h and term.h for header.h and io.c.
- tests/host/: the keys and the termcap strings checked on the build
  machine (43 checks), the patches applied to upstream, and upstream still
  built for Unix with them.
- amiga/: the Makefile, the GlowIcons (our own art), the Installer script
  and its spec. package/: the archive and the GFOpenUp part.

The AmiCradle Team.
