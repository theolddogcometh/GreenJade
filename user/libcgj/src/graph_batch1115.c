/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1115: extract IPv4 from IPv4-mapped IPv6.
 *
 * Surface (unique symbols):
 *   int gj_ipv6_v4mapped_extract(const uint8_t addr[16], uint8_t out[4]);
 *     — If addr is IPv4-mapped (::ffff:0:0/96), copy the embedded
 *       IPv4 address into out[0..3] and return 0. Else return -1
 *       without writing. Any NULL → -1.
 *   int __gj_ipv6_v4mapped_extract  (alias)
 *   __libcgj_batch1115_marker = "libcgj-batch1115"
 *
 * Distinct from gj_ipv6_is_v4mapped (batch1114) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1115_marker[] = "libcgj-batch1115";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_v4mapped_extract — pull a.b.c.d from ::ffff:a.b.c.d.
 *
 * Returns 0 on success with out filled; -1 if not v4-mapped or NULL.
 */
int
gj_ipv6_v4mapped_extract(const uint8_t aAddr[16], uint8_t aOut[4])
{
	unsigned i;

	if (aAddr == NULL || aOut == NULL) {
		return -1;
	}

	for (i = 0u; i < 10u; i++) {
		if (aAddr[i] != 0u) {
			return -1;
		}
	}
	if (aAddr[10] != 0xffu || aAddr[11] != 0xffu) {
		return -1;
	}

	aOut[0] = aAddr[12];
	aOut[1] = aAddr[13];
	aOut[2] = aAddr[14];
	aOut[3] = aAddr[15];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_v4mapped_extract(const uint8_t aAddr[16], uint8_t aOut[4])
    __attribute__((alias("gj_ipv6_v4mapped_extract")));
