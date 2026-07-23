/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch486: freestanding 16-bit host-to-network swap.
 *
 * Surface (unique symbols):
 *   uint16_t gj_htons16(uint16_t x);
 *     — Always byte-swap for little-endian host assumption (portable bswap).
 *       Network order is big-endian; on LE hosts htons is a pure swap.
 *   uint16_t __gj_htons16  (alias)
 *   __libcgj_batch486_marker = "libcgj-batch486"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from htons in byteorder.c.
 */

#include <stdint.h>

const char __libcgj_batch486_marker[] = "libcgj-batch486";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_htons16 — 16-bit host-to-network (always swap under LE host assumption).
 *
 * Portable pure-C bswap of the two octets. No endian probes, no builtins.
 */
uint16_t
gj_htons16(uint16_t x)
{
	return (uint16_t)(((x & 0x00ffu) << 8) | ((x & 0xff00u) >> 8));
}

uint16_t __gj_htons16(uint16_t x)
    __attribute__((alias("gj_htons16")));
