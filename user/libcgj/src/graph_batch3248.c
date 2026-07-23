/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3248: product bar3 full-ready predicate (u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_full_p_u(uint32_t mask);
 *     - Return 1 if mask has all three product bar3 readiness bits set
 *       (Steam/Deck/Top50: bits 0..2), i.e. (mask & 0x7) == 0x7.
 *       Else 0. Soft pure-data completeness gate. Unsigned form.
 *   uint32_t __gj_bar3_full_p_u  (alias)
 *   __libcgj_batch3248_marker = "libcgj-batch3248"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Unique
 * gj_bar3_full_p_u surface only; no multi-def. Distinct from
 * gj_bar3_ready_hint (batch2299 int), gj_bar3_mask_all_u (batch2895
 * four-slot), gj_bar3_partial_u (batch3099), and gj_bar3_bits_u
 * (batch3247 extract). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3248_marker[] = "libcgj-batch3248";

/* Three product bar3 readiness bits (bits 0..2). */
#define B3248_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if all three bar3 readiness bits are present in mask. */
static uint32_t
b3248_full_p(uint32_t uMask)
{
	if ((uMask & B3248_BAR3_MASK) == B3248_BAR3_MASK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_full_p_u - 1u iff bar3 readiness mask is fully set.
 *
 * mask: product bar3 readiness bitmask (bits 0..2 = Steam/Deck/Top50)
 *
 * Returns 1 when (mask & 0x7) == 0x7, else 0. Extra high bits are
 * ignored. Soft pure-data only; does not probe Steam/Deck. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_bar3_full_p_u(uint32_t uMask)
{
	(void)NULL;
	return b3248_full_p(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_full_p_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_full_p_u")));
