/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4469: freestanding 32-bit network-to-host (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ntohl_u(uint32_t x);
 *     - Always 4-octet byte-swap for little-endian host assumption
 *       (portable bswap). Symmetric with gj_htonl_u on LE hosts.
 *   uint32_t __gj_ntohl_u  (alias)
 *   __libcgj_batch4469_marker = "libcgj-batch4469"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_ntohl32 (batch489), ntohl (byteorder.c) — unique gj_ntohl_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4469_marker[] = "libcgj-batch4469";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4469_ntohl(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) | ((u32X & 0x0000ff00u) << 8) |
	    ((u32X & 0x00ff0000u) >> 8) | ((u32X >> 24) & 0x000000ffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntohl_u - 32-bit network-to-host (always swap under LE assumption).
 *
 * x: network-order 32-bit value
 *
 * Returns host-order 32-bit value. Portable pure-C bswap; no endian
 * probes, no builtins. Self-contained; no parent wires.
 */
uint32_t
gj_ntohl_u(uint32_t x)
{
	(void)NULL;
	return b4469_ntohl(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ntohl_u(uint32_t x)
    __attribute__((alias("gj_ntohl_u")));
