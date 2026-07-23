/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5067: freestanding 16-bit host-to-network.
 *
 * Surface (unique symbols):
 *   uint16_t gj_net_htons_u(uint16_t x);
 *     - Always byte-swap under little-endian host assumption
 *       (portable pure-C bswap of two octets). Network order is BE.
 *   uint16_t __gj_net_htons_u  (alias)
 *   __libcgj_batch5067_marker = "libcgj-batch5067"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_net_htons_u16 (batch1101) / gj_htons16 (batch486) / htons —
 * unique gj_net_htons_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5067_marker[] = "libcgj-batch5067";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b5067_htons(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_htons_u - 16-bit host-to-network (LE host: pure octet swap).
 *
 * x: host-order 16-bit value
 *
 * Returns network (big-endian) octet order as uint16_t integer form.
 * Self-contained; no parent wires.
 */
uint16_t
gj_net_htons_u(uint16_t x)
{
	(void)NULL;
	return b5067_htons(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_net_htons_u(uint16_t x)
    __attribute__((alias("gj_net_htons_u")));
