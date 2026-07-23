/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2398: product bar3 readiness bit extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_bar3_bits(uint32_t mask);
 *     - Return the low three product bar3 readiness bits of mask
 *       (mask & 0x7). Soft pure-data extract for the third product bar
 *       wave; not a runtime capability probe.
 *   uint32_t __gj_product_bar3_bits  (alias)
 *   __libcgj_batch2398_marker = "libcgj-batch2398"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_product_bar3_bits surface only; no multi-def. Distinct from
 * gj_bar3_ready_hint (batch2299 completeness predicate) and
 * gj_product_bar_steam_hint / gj_product_bar_deck_hint (batch1991/1992).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2398_marker[] = "libcgj-batch2398";

/* Three product bar3 readiness bits (bits 0..2). */
#define B2398_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract bar3 readiness bits from mask. */
static uint32_t
b2398_bits(uint32_t uMask)
{
	return uMask & B2398_BAR3_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar3_bits - extract low three product bar3 readiness bits.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask & 0x7. Pair with gj_bar3_ready_hint for completeness
 * (all three bits set). Does not call libc.
 */
uint32_t
gj_product_bar3_bits(uint32_t uMask)
{
	(void)NULL;
	return b2398_bits(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_bar3_bits(uint32_t uMask)
    __attribute__((alias("gj_product_bar3_bits")));
