/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1368: exponential-decay weight after n steps.
 *
 * Surface (unique symbols):
 *   uint32_t gj_exp_decay_weight(uint32_t alpha_milli, uint32_t n);
 *     — Weight of a sample n steps ago under EWMA with α in milli
 *       (0..1000): w = (1 - α)^n, returned in milli (0..1000).
 *       alpha_milli > 1000 treated as 1000. n == 0 → 1000.
 *   uint32_t __gj_exp_decay_weight  (alias)
 *   __libcgj_batch1368_marker = "libcgj-batch1368"
 *
 * Pair with gj_ewma_alpha_from_halflife (batch1367). Unique surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1368_marker[] = "libcgj-batch1368";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Compute (base_milli / 1000)^n * 1000 via binary exponentiation in
 * milli fixed-point. Intermediate product uses 64-bit then /1000.
 */
static uint32_t
b1368_pow_milli(uint32_t u32BaseMilli, uint32_t u32N)
{
	uint64_t u64R;
	uint64_t u64B;

	if (u32N == 0u) {
		return 1000u;
	}
	if (u32BaseMilli == 0u) {
		return 0u;
	}
	if (u32BaseMilli >= 1000u) {
		return 1000u;
	}

	u64R = 1000ull;
	u64B = (uint64_t)u32BaseMilli;
	while (u32N > 0u) {
		if ((u32N & 1u) != 0u) {
			u64R = (u64R * u64B) / 1000ull;
		}
		u32N >>= 1;
		if (u32N > 0u) {
			u64B = (u64B * u64B) / 1000ull;
		}
	}
	if (u64R > 1000ull) {
		return 1000u;
	}
	return (uint32_t)u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_exp_decay_weight — (1 - α)^n in milli-units.
 *
 * u32AlphaMilli: α * 1000 (clamped to 1000).
 * u32N:          steps since the sample.
 */
uint32_t
gj_exp_decay_weight(uint32_t u32AlphaMilli, uint32_t u32N)
{
	uint32_t u32Base;

	if (u32N == 0u) {
		return 1000u;
	}
	if (u32AlphaMilli >= 1000u) {
		return 0u;
	}
	u32Base = 1000u - u32AlphaMilli;
	return b1368_pow_milli(u32Base, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_exp_decay_weight(uint32_t u32AlphaMilli, uint32_t u32N)
    __attribute__((alias("gj_exp_decay_weight")));
