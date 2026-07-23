/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5068: freestanding 16-bit network-to-host.
 *
 * Surface (unique symbols):
 *   uint16_t gj_net_ntohs_u(uint16_t x);
 *     - Always byte-swap under little-endian host assumption
 *       (portable pure-C bswap of two octets). Network order is BE.
 *   uint16_t __gj_net_ntohs_u  (alias)
 *   __libcgj_batch5068_marker = "libcgj-batch5068"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_net_ntohs_u16 (batch1102) / gj_net_htons_u (batch5067) —
 * unique gj_net_ntohs_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5068_marker[] = "libcgj-batch5068";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b5068_ntohs(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ntohs_u - 16-bit network-to-host (LE host: pure octet swap).
 *
 * x: network-order 16-bit value
 *
 * Returns host (little-endian assumed) octet order as uint16_t.
 * Self-contained; no parent wires.
 */
uint16_t
gj_net_ntohs_u(uint16_t x)
{
	(void)NULL;
	return b5068_ntohs(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_net_ntohs_u(uint16_t x)
    __attribute__((alias("gj_net_ntohs_u")));
