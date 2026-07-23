/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch541: freestanding IPv6 inclusive range test.
 *
 * Surface (unique symbols):
 *   int gj_ipv6_in_range(const uint8_t addr[16],
 *                        const uint8_t lo[16],
 *                        const uint8_t hi[16]);
 *     — Treat each argument as a 128-bit IPv6 address in network
 *       (big-endian) byte order. Return 1 iff lo <= addr <= hi as
 *       unsigned 128-bit integers; else 0. Any NULL pointer → 0.
 *   int __gj_ipv6_in_range  (alias)
 *   __libcgj_batch541_marker = "libcgj-batch541"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch541_marker[] = "libcgj-batch541";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b541_cmp_be128 — compare two 16-byte big-endian values.
 * Returns -1 if a < b, 0 if a == b, +1 if a > b.
 */
static int
b541_cmp_be128(const uint8_t a[16], const uint8_t b[16])
{
	size_t i;

	for (i = 0u; i < 16u; i++) {
		if (a[i] < b[i]) {
			return -1;
		}
		if (a[i] > b[i]) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_in_range — inclusive IPv6 range membership (big-endian bytes).
 *
 * Edge cases:
 *   any of addr/lo/hi is NULL → 0
 *   lo > hi (empty range)     → always 0
 *   single-address (lo == hi) → 1 iff addr matches
 *   Returns 1 when lo <= addr <= hi, 0 otherwise.
 */
int
gj_ipv6_in_range(const uint8_t addr[16],
                 const uint8_t lo[16],
                 const uint8_t hi[16])
{
	if (addr == NULL || lo == NULL || hi == NULL) {
		return 0;
	}

	if (b541_cmp_be128(addr, lo) < 0) {
		return 0;
	}
	if (b541_cmp_be128(addr, hi) > 0) {
		return 0;
	}
	return 1;
}

int __gj_ipv6_in_range(const uint8_t addr[16],
                       const uint8_t lo[16],
                       const uint8_t hi[16])
    __attribute__((alias("gj_ipv6_in_range")));
