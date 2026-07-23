/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4225: freestanding last-byte index search (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_rfind_byte_u(const void *p, size_t n, uint8_t b);
 *     - Return the highest index i in [0, n) with p[i] == b, or n if no
 *       match. n == 0 -> 0 (empty window). If n > 0 and p is NULL -> n.
 *   size_t __gj_mem_rfind_byte_u  (alias)
 *   __libcgj_batch4225_marker = "libcgj-batch4225"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Unique gj_mem_rfind_byte_u surface only; no multi-def.
 * Companion reverse of gj_mem_find_byte_u (batch4224). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4225_marker[] = "libcgj-batch4225";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Highest index of needle in pU[0..n), or n if absent.
 * Caller: pU non-NULL, n > 0.
 */
static size_t
b4225_rfind(const unsigned char *pU, size_t n, unsigned char needle)
{
	size_t i;

	i = n;
	while (i > 0u) {
		i--;
		if (pU[i] == needle) {
			return i;
		}
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_rfind_byte_u - last index of octet b in p[0..n), or n if none.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> n)
 * n: number of octets to scan (0 -> 0)
 * b: target byte value
 *
 * Does not call libc. Returns size_t index (not a pointer).
 * No parent wires.
 */
size_t
gj_mem_rfind_byte_u(const void *p, size_t n, uint8_t b)
{
	(void)NULL;
	if (n == 0u) {
		return 0u;
	}
	if (p == NULL) {
		return n;
	}
	return b4225_rfind((const unsigned char *)p, n, (unsigned char)b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_rfind_byte_u(const void *p, size_t n, uint8_t b)
    __attribute__((alias("gj_mem_rfind_byte_u")));
