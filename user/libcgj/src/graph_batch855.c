/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch855: host-order netmask → CIDR prefix (or -1).
 *
 * Surface (unique symbols):
 *   int gj_prefix_from_netmask(uint32_t mask);
 *     — Contiguous left-aligned ones → 0..32. Non-contiguous → -1.
 *   int __gj_prefix_from_netmask  (alias)
 *   __libcgj_batch855_marker = "libcgj-batch855"
 *
 * Host-order input. Distinct from gj_netmask_to_prefix (BE) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch855_marker[] = "libcgj-batch855";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b855_popcount32(uint32_t u32X)
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
 * gj_prefix_from_netmask — CIDR length from host-order IPv4 netmask.
 *
 * Valid masks are 0 or left-aligned ones. Test:
 *   (mask | (mask - 1)) == 0xffffffff
 * (true for mask==0 via wrap of mask-1).
 */
int
gj_prefix_from_netmask(uint32_t u32Mask)
{
	if ((u32Mask | (u32Mask - 1u)) != 0xffffffffu) {
		return -1;
	}
	return b855_popcount32(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_prefix_from_netmask(uint32_t u32Mask)
    __attribute__((alias("gj_prefix_from_netmask")));
