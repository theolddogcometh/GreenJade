/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5718: Steam client ready-bits low-three pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_client_ready_bits_5718(uint32_t mask);
 *     - Return the low three readiness bits of mask (mask & 0x7):
 *       bit0 Steam client, bit1 Deck, bit2 Top50. Soft pure-data pack
 *       extract for bar3 Steam-client continuum wiring.
 *   uint32_t __gj_steam_client_ready_bits_5718  (alias)
 *   __libcgj_batch5718_marker = "libcgj-batch5718"
 *
 * Exclusive CREATE-ONLY (5711-5720: bar3 steam client ready bits).
 * Unique gj_steam_client_ready_bits_5718 surface only; no multi-def.
 * Distinct from gj_product_bar3_bits (batch2398), gj_bar3_ready_hint
 * (batch2299), and gj_bar3_client_ready_mask_5716 (batch5716 mask).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5718_marker[] = "libcgj-batch5718";

/* Low three bar3 readiness bits (Steam client / Deck / Top50). */
#define B5718_READY_BITS_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5718_ready_bits(uint32_t uMask)
{
	return uMask & B5718_READY_BITS_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_client_ready_bits_5718 - extract low-three ready bits.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask & 0x7 (bits 0..2). Soft pure-data pack extract; does
 * not call libc. No parent wires.
 */
uint32_t
gj_steam_client_ready_bits_5718(uint32_t uMask)
{
	(void)NULL;
	return b5718_ready_bits(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_client_ready_bits_5718(uint32_t uMask)
    __attribute__((alias("gj_steam_client_ready_bits_5718")));
