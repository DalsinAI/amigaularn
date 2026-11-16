#!/bin/sh
# Host tests of the Amiga port of uLarn:
#  1. amiga/amiga_keys.c and amiga/amiga_term.c (tests/host/test_keys.c):
#     the console's keys as uLarn reads them, the screen strings it writes;
#  2. amiga/patches apply to upstream at the locked commit, and upstream
#     with them still builds on the host (the patches only add
#     __amigaos__ branches; the Unix build is left as it was).
#   sh tests/host/test_amiga.sh        (fetches upstream at the lock)
#   UL=/path/to/ularn sh tests/host/test_amiga.sh
# MIT, Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/../.." && pwd)
. "$ROOT/package/ularn.lock"
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
CC=${CC:-cc}

$CC -std=c99 -Wall -Wextra -I"$ROOT/amiga" -o "$T/test_keys" \
    "$HERE/test_keys.c" "$ROOT/amiga/amiga_keys.c" "$ROOT/amiga/amiga_term.c"
"$T/test_keys"

if [ -n "${UL:-}" ]; then
    cp -R "$UL" "$T/ul"
else
    git -C "$T" init -q ul
    git -C "$T/ul" fetch -q --depth 1 "$UL_URL" "$UL_COMMIT"
    git -C "$T/ul" checkout -q FETCH_HEAD
fi
git -C "$T/ul" checkout -q -- . 2>/dev/null || true
for p in "$ROOT"/amiga/patches/*.patch; do
    (cd "$T/ul" && patch -s -p1 --dry-run < "$p" && patch -s -p1 < "$p")
done
echo "patches: $(ls "$ROOT"/amiga/patches/*.patch | wc -l) apply to $UL_COMMIT"
if command -v autoreconf >/dev/null 2>&1 && [ -n "$(command -v ncurses6-config || ls /usr/include/term.h 2>/dev/null)" ]; then
    host_build() {
        (cd "$1" && autoreconf -fi >/dev/null 2>&1 && CFLAGS="-std=gnu89 -w" ./configure -q >/dev/null 2>&1 && make -s -C src >/dev/null 2>&1)
    }
    if host_build "$T/ul"; then
        echo "host build with the patches: ok"
    else
        # upstream's tty.c needs termio.h or sgtty.h; glibc 2.42 has neither,
        # so check that unpatched upstream builds here before blaming the patches
        cp -R "$T/ul" "$T/ul0"
        (cd "$T/ul0" && make -s distclean >/dev/null 2>&1; \
         for p in $(ls -r "$ROOT"/amiga/patches/*.patch); do patch -s -R -p1 < "$p"; done) || exit 1
        if host_build "$T/ul0"; then
            echo "FAIL: upstream with the patches does not build on the host"; exit 1
        fi
        echo "host build skipped: unpatched upstream does not build on this host either"
    fi
else
    echo "host build skipped: no autoreconf or ncurses headers here"
fi
