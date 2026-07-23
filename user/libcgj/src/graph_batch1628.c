/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1628: fixed-span in-place ASCII uppercase (mem).
 *
 * Surface (unique symbols):
 *   void *gj_mem_to_upper_ascii_n(void *p, size_t n);
 *     — Convert each a..z octet in the first n bytes of p to A..Z in
 *       place. Does not stop at NUL; does not require a terminator.
 *       Returns p. NULL p → NULL. n == 0 → return p unchanged.
 *   void *__gj_mem_to_upper_ascii_n  (alias)
 *   __libcgj_batch1628_marker = "libcgj-batch1628"
 *
 * Distinct from gj_ascii_toupper_n / gj_str_to_upper_ascii_n (unique
 * gj_mem_ name; always processes exactly n octets). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1628_marker[] = "libcgj-batch1628";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned char
b1628_to_upper(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'a' && u8Ch <= (unsigned char)'z') {
		return (unsigned char)(u8Ch - (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_to_upper_ascii_n — a–z → A–Z over exactly n octets.
 *
 * p: mutable buffer of at least n bytes when n > 0 (NULL → NULL)
 * n: number of octets to process (0 → no-op, return p)
 *
 * Walks exactly n bytes (NUL is not a stop). Only 0x61..0x7A change.
 */
void *
gj_mem_to_upper_ascii_n(void *p, size_t n)
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
		pCh[iByte] = b1628_to_upper(pCh[iByte]);
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_to_upper_ascii_n(void *p, size_t n)
    __attribute__((alias("gj_mem_to_upper_ascii_n")));
