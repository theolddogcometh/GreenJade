/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2456: host-order IPv4 netmask to CIDR prefix.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_netmask_prefix(uint32_t mask);
 *     - Convert a host-order IPv4 netmask to a CIDR prefix length 0..32.
 *       Returns -1 if the mask is non-contiguous (not a left-aligned run
 *       of 1-bits followed by 0-bits in host bit order).
 *   int __gj_ipv4_netmask_prefix  (alias)
 *   __libcgj_batch2456_marker = "libcgj-batch2456"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_netmask_to_prefix
 * (batch336, network-order mask). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2456_marker[] = "libcgj-batch2456";

/* ---- freestanding helpers ---------------------------------------------- */

/* SWAR popcount of a 32-bit word (no __builtin dependency). */
static int
b2456_popcount32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return (int)(u32X & 0x3fu);
}

/*
 * Contiguous left-aligned ones (or zero) check:
 * (M | (M - 1)) == 0xffffffff. Holds for M == 0 via wrap of M - 1.
 */
static int
b2456_mask_ok(uint32_t u32Mask)
{
	if ((u32Mask | (u32Mask - 1u)) != 0xffffffffu) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_netmask_prefix - CIDR length from a host-order IPv4 netmask.
 *
 * mask: netmask in host integer order (e.g. 255.255.255.0 -> 0xffffff00).
 *
 * Returns:
 *   0..32  contiguous mask prefix length
 *  -1      non-contiguous / invalid mask bit pattern
 *
 * Does not call libc.
 */
int
gj_ipv4_netmask_prefix(uint32_t u32Mask)
{
	(void)NULL;
	if (!b2456_mask_ok(u32Mask)) {
		return -1;
	}
	return b2456_popcount32(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_netmask_prefix(uint32_t u32Mask)
    __attribute__((alias("gj_ipv4_netmask_prefix")));
