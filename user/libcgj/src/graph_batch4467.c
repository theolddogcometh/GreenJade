/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4467: freestanding 16-bit network-to-host (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_ntohs_u(uint16_t x);
 *     - Always byte-swap for little-endian host assumption (portable
 *       bswap). Symmetric with gj_htons_u on LE hosts.
 *   uint16_t __gj_ntohs_u  (alias)
 *   __libcgj_batch4467_marker = "libcgj-batch4467"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_ntohs16 (batch488), ntohs (byteorder.c) — unique gj_ntohs_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4467_marker[] = "libcgj-batch4467";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4467_ntohs(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntohs_u - 16-bit network-to-host (always swap under LE assumption).
 *
 * x: network-order 16-bit value
 *
 * Returns host-order 16-bit value. Portable pure-C bswap; no endian
 * probes, no builtins. Self-contained; no parent wires.
 */
uint16_t
gj_ntohs_u(uint16_t x)
{
	(void)NULL;
	return b4467_ntohs(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_ntohs_u(uint16_t x)
    __attribute__((alias("gj_ntohs_u")));
