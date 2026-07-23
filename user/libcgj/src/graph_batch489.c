/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch489: freestanding 32-bit network-to-host swap.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ntohl32(uint32_t x);
 *     — Same as htonl under LE host assumption: always portable bswap.
 *       Network order is big-endian; ntoh/hton are symmetric on LE hosts.
 *   uint32_t __gj_ntohl32  (alias)
 *   __libcgj_batch489_marker = "libcgj-batch489"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from ntohl in byteorder.c
 * and from gj_htonl32 in graph_batch487.c (unique symbol names).
 */

#include <stdint.h>

const char __libcgj_batch489_marker[] = "libcgj-batch489";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntohl32 — 32-bit network-to-host (always swap under LE host assumption).
 *
 * Identical transform to htonl on little-endian hosts: portable pure-C
 * bswap of the four octets. Self-contained (no cross-batch call).
 */
uint32_t
gj_ntohl32(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) |
	       ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) |
	       ((x & 0xff000000u) >> 24);
}

uint32_t __gj_ntohl32(uint32_t x)
    __attribute__((alias("gj_ntohl32")));
