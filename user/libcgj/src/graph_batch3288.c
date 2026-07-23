/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3288: last substring index (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_last_index_of_u(const char *hay, const char *needle);
 *     - Return the highest index i where needle occurs in hay, or
 *       (size_t)-1 if none / NULL args. Empty needle -> strlen(hay)
 *       when hay is non-NULL (match at end position).
 *   size_t __gj_str_last_index_of_u  (alias)
 *   __libcgj_batch3288_marker = "libcgj-batch3288"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_index_of_u (batch3287 first-index) and find_last_of charset
 * forms — unique last_index_of_u; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3288_marker[] = "libcgj-batch3288";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3288_strlen(const char *sz)
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
b3288_match_at(const char *pHay, size_t iHay, const char *pNeedle,
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
b3288_last_index_of(const char *szHay, const char *szNeedle)
{
size_t cbHay;
size_t cbNeed;
size_t iHay;
size_t iFound;
int fFound;

if (szHay == NULL || szNeedle == NULL) {
return (size_t)-1;
}

cbNeed = b3288_strlen(szNeedle);
cbHay = b3288_strlen(szHay);
if (cbNeed == 0u) {
return cbHay;
}
if (cbHay < cbNeed) {
return (size_t)-1;
}

fFound = 0;
iFound = 0u;
iHay = 0u;
while (iHay + cbNeed <= cbHay) {
if (b3288_match_at(szHay, iHay, szNeedle, cbNeed) != 0) {
fFound = 1;
iFound = iHay;
}
iHay++;
}
if (fFound == 0) {
return (size_t)-1;
}
return iFound;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_last_index_of_u - last index of needle in hay, or (size_t)-1.
 *
 * hay:    NUL-terminated haystack; NULL -> (size_t)-1
 * needle: NUL-terminated needle; NULL -> (size_t)-1; empty -> strlen(hay)
 *
 * Scans all alignments left-to-right, retaining the rightmost match.
 * No parent wires.
 */
size_t
gj_str_last_index_of_u(const char *hay, const char *needle)
{
(void)NULL;
return b3288_last_index_of(hay, needle);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_last_index_of_u(const char *hay, const char *needle)
    __attribute__((alias("gj_str_last_index_of_u")));
