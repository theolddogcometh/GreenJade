/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch773: freestanding 16-bit byteswap.
 *
 * Surface (unique symbols):
 *   uint16_t gj_bswap_u16(uint16_t x);
 *     — Swap the two octets of x (host-order in, host-order out of
 *       the swapped layout).
 *   uint16_t __gj_bswap_u16  (alias)
 *   __libcgj_batch773_marker = "libcgj-batch773"
 *
 * Distinct from htons/ntohs (byteorder.c) and from gj_bswap_u32 /
 * gj_bswap_u64. Unique gj_bswap_u16 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch773_marker[] = "libcgj-batch773";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap_u16 — reverse octet order of a 16-bit value.
 *
 * x: value to swap
 * Returns (x << 8) | (x >> 8) with width preserved.
 */
uint16_t
gj_bswap_u16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0xffu) << 8) | ((u16X >> 8) & 0xffu));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bswap_u16(uint16_t u16X)
    __attribute__((alias("gj_bswap_u16")));
