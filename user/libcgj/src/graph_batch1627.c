/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1627: fixed-span in-place ASCII lowercase (mem).
 *
 * Surface (unique symbols):
 *   void *gj_mem_to_lower_ascii_n(void *p, size_t n);
 *     — Convert each A..Z octet in the first n bytes of p to a..z in
 *       place. Does not stop at NUL; does not require a terminator.
 *       Returns p. NULL p → NULL. n == 0 → return p unchanged.
 *   void *__gj_mem_to_lower_ascii_n  (alias)
 *   __libcgj_batch1627_marker = "libcgj-batch1627"
 *
 * Distinct from gj_ascii_tolower_n / gj_str_to_lower_ascii_n (unique
 * gj_mem_ name; always processes exactly n octets). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1627_marker[] = "libcgj-batch1627";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned char
b1627_to_lower(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_to_lower_ascii_n — A–Z → a–z over exactly n octets.
 *
 * p: mutable buffer of at least n bytes when n > 0 (NULL → NULL)
 * n: number of octets to process (0 → no-op, return p)
 *
 * Walks exactly n bytes (NUL is not a stop). Only 0x41..0x5A change.
 */
void *
gj_mem_to_lower_ascii_n(void *p, size_t n)
{
	unsigned char *pCh;
	size_t iByte;

	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}

	pCh = (unsigned char *)p;
	for (iByte = 0u; iByte < n; iByte++) {
		pCh[iByte] = b1627_to_lower(pCh[iByte]);
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_to_lower_ascii_n(void *p, size_t n)
    __attribute__((alias("gj_mem_to_lower_ascii_n")));
