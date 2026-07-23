/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch336: IPv4 netmask (network byte order) to
 * CIDR prefix length.
 *
 * Surface (unique symbols):
 *   int gj_netmask_to_prefix(uint32_t mask_be);
 *     — Convert a 32-bit IPv4 netmask given in big-endian / network
 *       byte order (same layout as in_addr.s_addr) into a CIDR prefix
 *       length in 0..32. Returns -1 if the mask is non-contiguous
 *       (not a left-aligned run of 1-bits followed by 0-bits in host
 *       bit order after BE→host conversion).
 *   __gj_netmask_to_prefix  (alias)
 *   __libcgj_batch336_marker = "libcgj-batch336"
 *
 * Contiguity (host-order mask M):
 *   Valid masks are 0, or form 1...10...0 (left-aligned ones).
 *   Equivalent test: (M | (M - 1)) == 0xffffffff (holds for M == 0
 *   via wrap of M - 1). Prefix length is the popcount of M.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc byteorder helpers. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch336_marker[] = "libcgj-batch336";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Big-endian uint32 wire value → host-order numerical value (portable
 * bswap; GreenJade is LE x86_64, and this matches ntohl semantics).
 */
static uint32_t
b336_be_to_host(uint32_t u32Be)
{
	return ((u32Be & 0x000000ffu) << 24) |
	       ((u32Be & 0x0000ff00u) << 8) |
	       ((u32Be & 0x00ff0000u) >> 8) |
	       ((u32Be & 0xff000000u) >> 24);
}

/* SWAR popcount of a 32-bit word (no __builtin dependency). */
static int
b336_popcount32(uint32_t u32X)
{
	u32X = u32X - ((u32X >> 1) & 0x55555555u);
	u32X = (u32X & 0x33333333u) + ((u32X >> 2) & 0x33333333u);
	u32X = (u32X + (u32X >> 4)) & 0x0f0f0f0fu;
	u32X = u32X + (u32X >> 8);
	u32X = u32X + (u32X >> 16);
	return (int)(u32X & 0x3fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_netmask_to_prefix — CIDR length from a network-order IPv4 netmask.
 *
 * mask_be: netmask as big-endian 32-bit word (e.g. s_addr of ifa_netmask).
 *
 * Returns:
 *   0..32  contiguous mask prefix length
 *  -1      non-contiguous / invalid mask bit pattern
 */
int
gj_netmask_to_prefix(uint32_t mask_be)
{
	uint32_t u32Host;

	u32Host = b336_be_to_host(mask_be);

	/*
	 * Left-aligned ones (or zero): filling every bit below the lowest
	 * set bit must produce all-ones. Rejects holes and right-aligned
	 * (host-order) ones that are not valid netmasks.
	 */
	if ((u32Host | (u32Host - 1u)) != 0xffffffffu) {
		return -1;
	}

	return b336_popcount32(u32Host);
}

int __gj_netmask_to_prefix(uint32_t mask_be)
    __attribute__((alias("gj_netmask_to_prefix")));
