/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4086: round unsigned Q16.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_round_q16_u(uint32_t q);
 *     - Nearest integer of unsigned Q16.16 q; half rounds up
 *       (fraction >= 0.5 → +1). Max result 0x10000 when q >= 0xFFFF8000.
 *   uint32_t __gj_u32_round_q16_u  (alias)
 *   __libcgj_batch4086_marker = "libcgj-batch4086"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_q16_to_int_round (batch1475) / gj_u32_round_div_u (batch3855) —
 * unique gj_u32_round_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4086_marker[] = "libcgj-batch4086";

/* Low 16 bits = fractional field; half unit = 0.5 in Q16.16. */
#define B4086_FRAC_MASK  0xFFFFu
#define B4086_HALF       0x8000u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * round half-up: floor + 1 when frac >= 0x8000.
 * Avoids (q + 0x8000) overflow near UINT32_MAX.
 */
static uint32_t
b4086_round(uint32_t u32Q)
{
	uint32_t u32I;
	uint32_t u32F;

	u32I = u32Q >> 16;
	u32F = u32Q & B4086_FRAC_MASK;
	if (u32F >= B4086_HALF) {
		u32I += 1u;
	}
	return u32I;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_round_q16_u - nearest integer of unsigned Q16.16 q (half up).
 *
 * q: unsigned Q16.16 (bits 31..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns round(q) in [0, 0x10000]. No parent wires.
 */
uint32_t
gj_u32_round_q16_u(uint32_t u32Q)
{
	(void)NULL;
	return b4086_round(u32Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_round_q16_u(uint32_t u32Q)
    __attribute__((alias("gj_u32_round_q16_u")));
