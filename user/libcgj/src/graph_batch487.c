/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch487: freestanding 32-bit host-to-network swap.
 *
 * Surface (unique symbols):
 *   uint32_t gj_htonl32(uint32_t x);
 *     — Always byte-swap for little-endian host assumption (portable bswap).
 *       Network order is big-endian; on LE hosts htonl is a pure swap.
 *   uint32_t __gj_htonl32  (alias)
 *   __libcgj_batch487_marker = "libcgj-batch487"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from htonl in byteorder.c.
 */

#include <stdint.h>

const char __libcgj_batch487_marker[] = "libcgj-batch487";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_htonl32 — 32-bit host-to-network (always swap under LE host assumption).
 *
 * Portable pure-C bswap of the four octets. No endian probes, no builtins.
 */
uint32_t
gj_htonl32(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) |
	       ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) |
	       ((x & 0xff000000u) >> 24);
}

uint32_t __gj_htonl32(uint32_t x)
    __attribute__((alias("gj_htonl32")));
