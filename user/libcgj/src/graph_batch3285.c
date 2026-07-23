/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3285: C-string character membership (_u).
 *
 * Surface (unique symbols):
 *   int gj_str_contains_char_u(const char *s, int c);
 *     - Return 1 if (unsigned char)c occurs in the NUL-terminated
 *       string s (payload octets before the terminator). s == NULL -> 0.
 *       The terminating NUL itself is not treated as a match target
 *       for c == 0 (payload-only scan).
 *   int __gj_str_contains_char_u  (alias)
 *   __libcgj_batch3285_marker = "libcgj-batch3285"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290). Distinct from
 * gj_str_chr / gj_str_chr_u (pointer form) and gj_str_count_char
 * (batch604) — unique contains_char_u predicate; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3285_marker[] = "libcgj-batch3285";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3285_contains_char(const char *sz, int nC)
{
unsigned char ch;
const unsigned char *p;

if (sz == NULL) {
return 0;
}

ch = (unsigned char)nC;
p = (const unsigned char *)sz;
while (*p != '\0') {
if (*p == ch) {
return 1;
}
p++;
}
return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_contains_char_u - 1 if byte c occurs in s, else 0.
 *
 * s: NUL-terminated string, or NULL
 * c: search byte (truncated to unsigned char)
 *
 * Payload-only scan (stops at first NUL). No parent wires.
 */
int
gj_str_contains_char_u(const char *s, int c)
{
(void)NULL;
return b3285_contains_char(s, c);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_contains_char_u(const char *s, int c)
    __attribute__((alias("gj_str_contains_char_u")));
