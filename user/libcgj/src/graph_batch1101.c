/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1101: freestanding 16-bit host-to-network.
 *
 * Surface (unique symbols):
 *   uint16_t gj_net_htons_u16(uint16_t x);
 *     — Always byte-swap under little-endian host assumption
 *       (portable pure-C bswap of two octets). Network order is BE.
 *   uint16_t __gj_net_htons_u16  (alias)
 *   __libcgj_batch1101_marker = "libcgj-batch1101"
 *
 * Distinct from gj_htons16 (batch486) / htons — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1101_marker[] = "libcgj-batch1101";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_htons_u16 — 16-bit host-to-network (LE host: pure octet swap).
 */
uint16_t
gj_net_htons_u16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_net_htons_u16(uint16_t u16X)
    __attribute__((alias("gj_net_htons_u16")));
