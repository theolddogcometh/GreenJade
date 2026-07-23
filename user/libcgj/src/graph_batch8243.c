/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8243: freestanding 16-bit host-to-network (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_htons_u_8243(uint16_t x);
 *     - Always byte-swap for little-endian host assumption (portable
 *       bswap). Network order is big-endian; on LE hosts htons is a
 *       pure swap of the two octets.
 *   uint16_t __gj_htons_u_8243  (alias)
 *   __libcgj_batch8243_marker = "libcgj-batch8243"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Distinct from gj_htons16 (batch486), gj_htons_u (batch4466), htons
 * (byteorder.c) — unique gj_htons_u_8243 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8243_marker[] = "libcgj-batch8243";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b8243_htons(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_htons_u_8243 - 16-bit host-to-network (always swap under LE assumption).
 *
 * x: host-order 16-bit value
 *
 * Returns network-order 16-bit value. Portable pure-C bswap; no
 * endian probes, no builtins. Self-contained; no parent wires.
 */
uint16_t
gj_htons_u_8243(uint16_t x)
{
	(void)NULL;
	return b8243_htons(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_htons_u_8243(uint16_t x)
    __attribute__((alias("gj_htons_u_8243")));
