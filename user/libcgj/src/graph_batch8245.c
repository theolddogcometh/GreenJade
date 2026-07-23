/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8245: freestanding 32-bit host-to-network (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_htonl_u_8245(uint32_t x);
 *     - Always byte-swap for little-endian host assumption (portable
 *       bswap). Network order is big-endian; on LE hosts htonl is a
 *       pure swap of the four octets.
 *   uint32_t __gj_htonl_u_8245  (alias)
 *   __libcgj_batch8245_marker = "libcgj-batch8245"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Distinct from gj_htonl32 (batch487), gj_htonl_u (batch4468), htonl
 * (byteorder.c) — unique gj_htonl_u_8245 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8245_marker[] = "libcgj-batch8245";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8245_htonl(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_htonl_u_8245 - 32-bit host-to-network (always swap under LE assumption).
 *
 * x: host-order 32-bit value
 *
 * Returns network-order 32-bit value. Portable pure-C bswap; no
 * endian probes, no builtins. Self-contained; no parent wires.
 */
uint32_t
gj_htonl_u_8245(uint32_t x)
{
	(void)NULL;
	return b8245_htonl(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_htonl_u_8245(uint32_t x)
    __attribute__((alias("gj_htonl_u_8245")));
