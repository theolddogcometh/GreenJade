/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4087: round unsigned Q48.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_round_q16_u(uint64_t q);
 *     - Nearest integer of unsigned Q48.16 q; half rounds up
 *       (fraction >= 0.5 → +1). Max result 2^48 when frac is half-or-more
 *       on the top integer binade.
 *   uint64_t __gj_u64_round_q16_u  (alias)
 *   __libcgj_batch4087_marker = "libcgj-batch4087"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u32_round_q16_u (batch4086) / gj_u64_round_div_u (batch3856) —
 * unique gj_u64_round_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4087_marker[] = "libcgj-batch4087";

/* Low 16 bits = fractional field; half unit = 0.5 in Q16.16. */
#define B4087_FRAC_MASK  0xFFFFull
#define B4087_HALF       0x8000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * round half-up: floor + 1 when frac >= 0x8000.
 * Avoids (q + 0x8000) overflow near UINT64_MAX.
 */
static uint64_t
b4087_round(uint64_t u64Q)
{
	uint64_t u64I;
	uint64_t u64F;

	u64I = u64Q >> 16;
	u64F = u64Q & B4087_FRAC_MASK;
	if (u64F >= B4087_HALF) {
		u64I += 1ull;
	}
	return u64I;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_round_q16_u - nearest integer of unsigned Q48.16 q (half up).
 *
 * q: unsigned Q48.16 (bits 63..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns round(q) in [0, 2^48]. No parent wires.
 */
uint64_t
gj_u64_round_q16_u(uint64_t u64Q)
{
	(void)NULL;
	return b4087_round(u64Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_round_q16_u(uint64_t u64Q)
    __attribute__((alias("gj_u64_round_q16_u")));
