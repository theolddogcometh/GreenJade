/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch308: integer exponential moving average.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ema_u32(uint32_t prev, uint32_t sample,
 *                       uint32_t num, uint32_t den);
 *     — One EMA step with rational alpha = num/den (0 <= alpha <= 1 after
 *       clamp). Integer form:
 *         out = (prev * (den - num) + sample * num) / den
 *       Truncates toward zero (unsigned division). Uses uint64_t products
 *       so the multiplies never wrap a uint32_t intermediate.
 *       Edge cases:
 *         den == 0           → return prev (undefined alpha; keep state)
 *         num == 0           → return prev (alpha = 0)
 *         num >= den > 0     → num clamped to den (alpha = 1 → sample)
 *   __gj_ema_u32  (alias)
 *   __libcgj_batch308_marker = "libcgj-batch308"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch308_marker[] = "libcgj-batch308";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ema_u32 — integer EMA update: prev ← (1-α)·prev + α·sample.
 *
 * prev:   previous filtered state
 * sample: new observation
 * num:    numerator of α (clamped to den when num > den)
 * den:    denominator of α; zero leaves prev unchanged
 *
 * Equivalent rearrangements of the classic recurrence:
 *   out = prev + α·(sample − prev)
 *   out = (1−α)·prev + α·sample
 * with α = num/den in Q0 fixed-point rational form.
 */
uint32_t
gj_ema_u32(uint32_t uPrev, uint32_t uSample, uint32_t uNum, uint32_t uDen)
{
	uint64_t u64Num;
	uint64_t u64Den;
	uint64_t u64Acc;

	if (uDen == 0u) {
		return uPrev;
	}
	if (uNum == 0u) {
		return uPrev;
	}

	u64Num = (uint64_t)uNum;
	u64Den = (uint64_t)uDen;
	if (u64Num > u64Den) {
		u64Num = u64Den;
	}

	/*
	 * out = (prev * (den - num) + sample * num) / den
	 * Products fit in uint64_t: max is UINT32_MAX * UINT32_MAX.
	 */
	u64Acc = (uint64_t)uPrev * (u64Den - u64Num);
	u64Acc += (uint64_t)uSample * u64Num;
	return (uint32_t)(u64Acc / u64Den);
}

uint32_t __gj_ema_u32(uint32_t uPrev, uint32_t uSample, uint32_t uNum,
    uint32_t uDen)
    __attribute__((alias("gj_ema_u32")));
