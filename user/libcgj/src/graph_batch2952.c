/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2952: length-bounded in-place ASCII upper fold.
 *
 * Surface (unique symbols):
 *   void gj_str_toupper_n_u(char *s, size_t n);
 *     - Convert each a..z octet in the first n bytes of s to A..Z
 *       in place. Non-letters and non-ASCII bytes are left unchanged.
 *       Does not stop at NUL; does not require or write a terminator.
 *       No-op when s is NULL or n is 0.
 *   void __gj_str_toupper_n_u  (alias)
 *   __libcgj_batch2952_marker = "libcgj-batch2952"
 *
 * Post-2950 exclusive string helpers wave (2951-2960). Distinct from
 * gj_ascii_toupper_n (batch505) and gj_ascii_toupper (batch355) —
 * unique gj_str_toupper_n_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2952_marker[] = "libcgj-batch2952";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one unsigned octet: a..z → A..Z; else identity. */
static unsigned char
b2952_to_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - ((unsigned char)'a' -
		    (unsigned char)'A'));
	}
	return u8Ch;
}

static void
b2952_toupper_n(char *s, size_t n)
{
	unsigned char *pCh;
	size_t iByte;

	if (s == NULL || n == 0u) {
		return;
	}

	pCh = (unsigned char *)(void *)s;
	for (iByte = 0u; iByte < n; iByte++) {
		pCh[iByte] = b2952_to_upper(pCh[iByte]);
	}
	(void)(uint32_t)0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_toupper_n_u - fold ASCII lowercase letters in a fixed span.
 *
 * s: mutable buffer of at least n bytes (NULL → no-op)
 * n: number of octets to process (0 → no-op)
 *
 * Walks exactly n bytes (NUL is not a stop). Only a..z (0x61..0x7A)
 * are changed. Does not call libc.
 */
void
gj_str_toupper_n_u(char *s, size_t n)
{
	(void)NULL;
	b2952_toupper_n(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_str_toupper_n_u(char *s, size_t n)
    __attribute__((alias("gj_str_toupper_n_u")));
