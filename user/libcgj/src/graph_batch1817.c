/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1817: extract IPv4 from IPv4-mapped IPv6.
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_v4mapped_get(const uint8_t addr[16], uint8_t out[4]);
 *     — If addr is IPv4-mapped (::ffff:0:0/96), copy the embedded
 *       IPv4 address into out[0..3] and return 0. Else return -1
 *       without writing. Any NULL → -1.
 *   int __gj_ipv6_v4mapped_get  (alias)
 *   __libcgj_batch1817_marker = "libcgj-batch1817"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1817_marker[] = "libcgj-batch1817";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if aAddr has the ::ffff:0:0/96 prefix. */
static int
b1817_is_v4mapped(const uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 10u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	if (aAddr[10] != 0xffu || aAddr[11] != 0xffu) {
		return 0;
	}
	return 1;
}

/* Copy last 4 octets of aAddr into aOut[0..3]. */
static void
b1817_copy4(const uint8_t aAddr[16], uint8_t aOut[4])
{
	aOut[0] = aAddr[12];
	aOut[1] = aAddr[13];
	aOut[2] = aAddr[14];
	aOut[3] = aAddr[15];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_v4mapped_get — pull a.b.c.d from ::ffff:a.b.c.d.
 *
 * Returns 0 on success with out filled; -1 if not v4-mapped or NULL.
 * On failure out is left unmodified.
 */
int
gj_ipv6_v4mapped_get(const uint8_t aAddr[16], uint8_t aOut[4])
{
	if (aAddr == NULL || aOut == NULL) {
		return -1;
	}
	if (!b1817_is_v4mapped(aAddr)) {
		return -1;
	}
	b1817_copy4(aAddr, aOut);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_v4mapped_get(const uint8_t aAddr[16], uint8_t aOut[4])
    __attribute__((alias("gj_ipv6_v4mapped_get")));
