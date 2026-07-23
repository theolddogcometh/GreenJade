/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3286: C-string substring predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_contains_str_u(const char *hay, const char *needle);
 *     - Return 1 if needle occurs as a contiguous substring of hay,
 *       else 0. Empty needle -> 1 when hay is non-NULL. hay == NULL or
 *       needle == NULL -> 0.
 *   int __gj_str_contains_str_u  (alias)
 *   __libcgj_batch3286_marker = "libcgj-batch3286"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_contains (batch603) — unique contains_str_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3286_marker[] = "libcgj-batch3286";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3286_contains_str(const char *szHay, const char *szNeedle)
{
size_t i;
size_t j;

if (szHay == NULL || szNeedle == NULL) {
return 0;
}

/* Empty needle matches any non-NULL hay (like strstr empty). */
if (szNeedle[0] == '\0') {
return 1;
}

for (i = 0u; szHay[i] != '\0'; i++) {
if (szHay[i] != szNeedle[0]) {
continue;
}
j = 0u;
for (;;) {
if (szNeedle[j] == '\0') {
return 1;
}
if (szHay[i + j] == '\0' || szHay[i + j] != szNeedle[j]) {
break;
}
j++;
}
}
return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_contains_str_u - 1 if needle is a substring of hay, else 0.
 *
 * hay:    haystack C string (NULL -> 0)
 * needle: required substring (NULL -> 0; empty matches any non-NULL hay)
 *
 * Unsigned-byte exact compare. Does not call libc strstr. No parent wires.
 */
int
gj_str_contains_str_u(const char *hay, const char *needle)
{
(void)NULL;
return b3286_contains_str(hay, needle);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_contains_str_u(const char *hay, const char *needle)
    __attribute__((alias("gj_str_contains_str_u")));
