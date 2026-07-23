/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3247: product bar3 readiness bit extract (u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_bits_u(uint32_t mask);
 *     - Return the low three product bar3 readiness bits of mask
 *       (mask & 0x7). Soft pure-data extract for Steam/Deck/Top50
 *       slots on the milestone 3250 continuum. Unsigned form.
 *   uint32_t __gj_bar3_bits_u  (alias)
 *   __libcgj_batch3247_marker = "libcgj-batch3247"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Unique
 * gj_bar3_bits_u surface only; no multi-def. Distinct from
 * gj_product_bar3_bits (batch2398), gj_bar3_score_3200 (batch3198),
 * gj_bar3_partial_u (batch3099), and gj_bar3_full_p_u (batch3248).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3247_marker[] = "libcgj-batch3247";

/* Three product bar3 readiness bits (bits 0..2). */
#define B3247_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* Extract bar3 readiness bits from mask. */
static uint32_t
b3247_bits(uint32_t uMask)
{
	return uMask & B3247_BAR3_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_bits_u - extract low three product bar3 readiness bits.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask & 0x7. Pair with gj_bar3_full_p_u for completeness
 * (all three bits set). Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_bits_u(uint32_t uMask)
{
	(void)NULL;
	return b3247_bits(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_bits_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_bits_u")));
