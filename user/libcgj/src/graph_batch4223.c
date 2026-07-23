/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4223: freestanding count of matching octets (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_mem_count_byte_u(const void *p, size_t n, uint8_t b);
 *     - Count how many octets in p[0..n) equal b. n == 0 -> 0 even if
 *       p is NULL. If n > 0 and p is NULL -> 0. Binary-safe: n is the
 *       sole bound.
 *   size_t __gj_mem_count_byte_u  (alias)
 *   __libcgj_batch4223_marker = "libcgj-batch4223"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Unique gj_mem_count_byte_u surface only; no multi-def.
 * Distinct from gj_mem_count_byte_n (batch2023) / gj_mem_count_u8_u
 * (batch2764). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4223_marker[] = "libcgj-batch4223";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count octets equal to needle in pU[0..n). Caller: pU non-NULL when n>0. */
static size_t
b4223_count(const unsigned char *pU, size_t n, unsigned char needle)
{
	size_t i;
	size_t cHit;

	cHit = 0u;
	for (i = 0u; i < n; i++) {
		if (pU[i] == needle) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_count_byte_u - count octets equal to b in p[0..n).
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> 0)
 * n: number of octets to scan (0 -> 0)
 * b: target byte value
 *
 * Does not call libc.
 * No parent wires.
 */
size_t
gj_mem_count_byte_u(const void *p, size_t n, uint8_t b)
{
	(void)NULL;
	if (n == 0u || p == NULL) {
		return 0u;
	}
	return b4223_count((const unsigned char *)p, n, (unsigned char)b);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_mem_count_byte_u(const void *p, size_t n, uint8_t b)
    __attribute__((alias("gj_mem_count_byte_u")));
