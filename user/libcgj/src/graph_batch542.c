/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch542: IPv4 CIDR membership test (host-order).
 *
 * Surface (unique symbols):
 *   int gj_cidr_contains(uint32_t net, uint8_t prefix, uint32_t addr);
 *     — Return 1 if host-order IPv4 addr lies in network net/prefix,
 *       else 0. prefix > 32 yields 0. prefix 0 matches every addr.
 *   __gj_cidr_contains  (alias)
 *   __libcgj_batch542_marker = "libcgj-batch542"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch542_marker[] = "libcgj-batch542";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cidr_contains — true if host-order IPv4 addr is in net/prefix.
 *
 * net, addr: IPv4 addresses in host byte order (not network order).
 * prefix:    CIDR prefix length in bits (0..32 valid).
 *
 * prefix 0  → always 1 (0.0.0.0/0 matches all).
 * prefix >32 → always 0.
 * Otherwise  → 1 iff the high prefix bits of addr equal those of net.
 *
 * Mask construction avoids a 32-bit left shift of 32 (UB) by treating
 * prefix 0 as an empty mask before the shift path is used.
 */
int
gj_cidr_contains(uint32_t net, uint8_t prefix, uint32_t addr)
{
	uint32_t mask;

	if (prefix > 32u) {
		return 0;
	}
	if (prefix == 0u) {
		return 1;
	}

	/* prefix in 1..32: shift amount is 31..0 — well-defined on uint32. */
	mask = 0xffffffffu << (32u - (unsigned)prefix);
	return ((addr & mask) == (net & mask)) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cidr_contains(uint32_t net, uint8_t prefix, uint32_t addr)
    __attribute__((alias("gj_cidr_contains")));
