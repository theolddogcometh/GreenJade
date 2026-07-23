/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8246: freestanding 32-bit network-to-host (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ntohl_u_8246(uint32_t x);
 *     - Always byte-swap for little-endian host assumption (portable
 *       bswap). Symmetric with gj_htonl_u_8245 on LE hosts.
 *   uint32_t __gj_ntohl_u_8246  (alias)
 *   __libcgj_batch8246_marker = "libcgj-batch8246"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Distinct from gj_ntohl32 (batch489), gj_ntohl_u (batch4469), ntohl
 * (byteorder.c) — unique gj_ntohl_u_8246 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8246_marker[] = "libcgj-batch8246";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8246_ntohl(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntohl_u_8246 - 32-bit network-to-host (always swap under LE assumption).
 *
 * x: network-order 32-bit value
 *
 * Returns host-order 32-bit value. Identical transform to htonl on LE
 * hosts. Self-contained; no cross-batch call; no parent wires.
 */
uint32_t
gj_ntohl_u_8246(uint32_t x)
{
	(void)NULL;
	return b8246_ntohl(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ntohl_u_8246(uint32_t x)
    __attribute__((alias("gj_ntohl_u_8246")));
