/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1621: bounded in-place ASCII lowercase (C string).
 *
 * Surface (unique symbols):
 *   char *gj_str_to_lower_ascii_n(char *s, size_t n);
 *     — Convert each A..Z octet in s to a..z for at most n octets,
 *       stopping early at the first NUL. Non-letters left unchanged.
 *       Returns s. NULL s → NULL. n == 0 → return s unchanged.
 *   char *__gj_str_to_lower_ascii_n  (alias)
 *   __libcgj_batch1621_marker = "libcgj-batch1621"
 *
 * Distinct from gj_ascii_tolower / gj_ascii_tolower_n / gj_str_map_lower
 * (different names; this is n-capped NUL-stopping C-string walk).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1621_marker[] = "libcgj-batch1621";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned char
b1621_to_lower(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_to_lower_ascii_n — A–Z → a–z over a NUL-capped n-byte window.
 *
 * s: mutable C string (NULL → NULL)
 * n: maximum octets to examine (0 → no-op)
 *
 * Stops at the first 0 byte or after n octets, whichever comes first.
 * Only 0x41..0x5A are rewritten.
 */
char *
gj_str_to_lower_ascii_n(char *s, size_t n)
{
	unsigned char *pCh;
	size_t iByte;

	if (s == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return s;
	}

	pCh = (unsigned char *)s;
	for (iByte = 0u; iByte < n; iByte++) {
		if (pCh[iByte] == 0u) {
			break;
		}
		pCh[iByte] = b1621_to_lower(pCh[iByte]);
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_to_lower_ascii_n(char *s, size_t n)
    __attribute__((alias("gj_str_to_lower_ascii_n")));
