/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1818: IPv6 16-octet address equality.
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_addr_eq(const uint8_t a[16], const uint8_t b[16]);
 *     — Return 1 if a and b have identical 16 octets, else 0.
 *       Any NULL pointer → 0.
 *   int __gj_ipv6_addr_eq  (alias)
 *   __libcgj_batch1818_marker = "libcgj-batch1818"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1818_marker[] = "libcgj-batch1818";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if aA and aB match for all 16 octets. */
static int
b1818_eq16(const uint8_t aA[16], const uint8_t aB[16])
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (aA[i] != aB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_addr_eq — octet-wise equality of two IPv6 addresses.
 *
 * Edge cases:
 *   a == NULL or b == NULL → 0
 *   a and b both all-zero (::) → 1
 *   same pointer → 1 (if non-NULL)
 */
int
gj_ipv6_addr_eq(const uint8_t aA[16], const uint8_t aB[16])
{
	if (aA == NULL || aB == NULL) {
		return 0;
	}
	return b1818_eq16(aA, aB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_addr_eq(const uint8_t aA[16], const uint8_t aB[16])
    __attribute__((alias("gj_ipv6_addr_eq")));
