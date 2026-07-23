/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch335: IPv4 CIDR prefix match on big-endian
 * network-order uint32 addresses.
 *
 * Surface (unique symbols):
 *   int gj_cidr4_match(uint32_t ip_be, uint32_t net_be, unsigned prefix);
 *     — Test whether ip_be lies in the CIDR range net_be/prefix.
 *       Both addresses are big-endian network bit significance: the most
 *       significant bit of the uint32 is the first network bit (octet 0
 *       is bits 31..24). prefix is the number of leading bits that must
 *       match (0..32). Returns 1 if the masked addresses are equal,
 *       0 if they differ, and -1 if prefix > 32. prefix == 0 matches
 *       every address (empty mask). prefix == 32 requires full equality.
 *   __gj_cidr4_match  (alias)
 *   __libcgj_batch335_marker = "libcgj-batch335"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc byte-swap helpers. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch335_marker[] = "libcgj-batch335";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cidr4_match — CIDR membership for two network-order IPv4 addresses.
 *
 * Returns:
 *   1  ip_be is inside net_be/prefix
 *   0  ip_be is outside the range
 *  -1  bad prefix (prefix > 32)
 *
 * The mask is built in network bit significance so left-shifts never
 * shift by 32 (UB): prefix 0 uses mask 0; otherwise mask is the high
 * `prefix` bits set.
 */
int
gj_cidr4_match(uint32_t u32IpBe, uint32_t u32NetBe, unsigned uPrefix)
{
	uint32_t u32Mask;

	if (uPrefix > 32u) {
		return -1;
	}

	if (uPrefix == 0u) {
		u32Mask = 0u;
	} else {
		u32Mask = 0xffffffffu << (32u - uPrefix);
	}

	if ((u32IpBe & u32Mask) == (u32NetBe & u32Mask)) {
		return 1;
	}
	return 0;
}

int __gj_cidr4_match(uint32_t u32IpBe, uint32_t u32NetBe, unsigned uPrefix)
    __attribute__((alias("gj_cidr4_match")));
