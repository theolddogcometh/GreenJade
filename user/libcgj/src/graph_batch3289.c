/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3289: non-overlapping substring count (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_substr_u(const char *hay, const char *needle);
 *     - Count non-overlapping occurrences of needle in hay. After each
 *       match, search resumes past the matched needle (advance by
 *       needle length). Empty needle, NULL hay, or NULL needle yield 0.
 *   size_t __gj_str_count_substr_u  (alias)
 *   __libcgj_batch3289_marker = "libcgj-batch3289"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_count_substr (batch732) and gj_count_substr (batch359) —
 * unique count_substr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3289_marker[] = "libcgj-batch3289";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3289_strlen(const char *sz)
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
b3289_match_at(const char *pHay, size_t iHay, const char *pNeedle,
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
b3289_count_substr(const char *szHay, const char *szNeedle)
{
size_t cbHay;
size_t cbNeed;
size_t cHits;
size_t iHay;

if (szHay == NULL || szNeedle == NULL) {
return 0u;
}

cbNeed = b3289_strlen(szNeedle);
if (cbNeed == 0u) {
return 0u;
}

cbHay = b3289_strlen(szHay);
if (cbHay < cbNeed) {
return 0u;
}

cHits = 0u;
iHay = 0u;
while (iHay + cbNeed <= cbHay) {
if (b3289_match_at(szHay, iHay, szNeedle, cbNeed) != 0) {
cHits++;
/* Non-overlapping: skip the whole match. */
iHay += cbNeed;
} else {
iHay++;
}
}
return cHits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_substr_u - non-overlapping count of needle in hay.
 *
 * hay:    NUL-terminated haystack; NULL -> 0
 * needle: NUL-terminated needle; NULL or empty -> 0
 *
 * Advances past each full match by needle length. No parent wires.
 */
size_t
gj_str_count_substr_u(const char *hay, const char *needle)
{
(void)NULL;
return b3289_count_substr(hay, needle);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_substr_u(const char *hay, const char *needle)
    __attribute__((alias("gj_str_count_substr_u")));
