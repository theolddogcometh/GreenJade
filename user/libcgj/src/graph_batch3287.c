/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3287: first substring index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_index_of_u(const char *hay, const char *needle);
 *     - Return the lowest index i where needle occurs in hay, or
 *       (size_t)-1 if none / NULL args. Empty needle -> 0 when hay is
 *       non-NULL (match at position zero).
 *   size_t __gj_str_index_of_u  (alias)
 *   __libcgj_batch3287_marker = "libcgj-batch3287"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_find_first_of (batch1321 charset form) and pointer-returning
 * strstr surfaces — unique index_of_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3287_marker[] = "libcgj-batch3287";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3287_strlen(const char *sz)
{
size_t cb;

if (sz == NULL) {
return 0u;
}
cb = 0u;
while (sz[cb] != '\0') {
cb++;
}
return cb;
}

static int
b3287_match_at(const char *pHay, size_t iHay, const char *pNeedle,
    size_t cbNeed)
{
size_t i;

for (i = 0u; i < cbNeed; i++) {
if (pHay[iHay + i] != pNeedle[i]) {
return 0;
}
}
return 1;
}

static size_t
b3287_index_of(const char *szHay, const char *szNeedle)
{
size_t cbHay;
size_t cbNeed;
size_t iHay;

if (szHay == NULL || szNeedle == NULL) {
return (size_t)-1;
}

cbNeed = b3287_strlen(szNeedle);
if (cbNeed == 0u) {
return 0u;
}

cbHay = b3287_strlen(szHay);
if (cbHay < cbNeed) {
return (size_t)-1;
}

iHay = 0u;
while (iHay + cbNeed <= cbHay) {
if (b3287_match_at(szHay, iHay, szNeedle, cbNeed) != 0) {
return iHay;
}
iHay++;
}
return (size_t)-1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_index_of_u - first index of needle in hay, or (size_t)-1.
 *
 * hay:    NUL-terminated haystack; NULL -> (size_t)-1
 * needle: NUL-terminated needle; NULL -> (size_t)-1; empty -> 0
 *
 * Non-overlapping search from the left. No parent wires.
 */
size_t
gj_str_index_of_u(const char *hay, const char *needle)
{
(void)NULL;
return b3287_index_of(hay, needle);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_index_of_u(const char *hay, const char *needle)
    __attribute__((alias("gj_str_index_of_u")));
