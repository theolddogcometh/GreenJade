/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3586: ASCII case-fold one character (_u).
 *
 * Surface (unique symbols):
 *   unsigned char gj_ascii_fold_char_u(unsigned char c);
 *     - Fold ASCII uppercase A-Z to lowercase a-z; all other octets
 *       returned unchanged. Locale-independent pure scalar.
 *   unsigned char __gj_ascii_fold_char_u  (alias)
 *   __libcgj_batch3586_marker = "libcgj-batch3586"
 *
 * Exclusive continuum CREATE-ONLY (3581-3590). Distinct from
 * gj_ascii_tolower (batch355 in-place string) and ctype tolower —
 * unique fold_char_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3586_marker[] = "libcgj-batch3586";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b3586_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fold_char_u - fold one ASCII uppercase letter to lowercase.
 *
 * c: input octet
 *
 * Returns folded octet (A-Z -> a-z); identity for all other values.
 */
unsigned char
gj_ascii_fold_char_u(unsigned char u8C)
{
	(void)NULL;
	return b3586_fold(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned char __gj_ascii_fold_char_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_fold_char_u")));
