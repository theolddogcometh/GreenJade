/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch656: saturating uint64_t decrement.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_dec_sat(uint64_t x);
 *     — x - 1 with saturation at 0 on underflow (no wrap).
 *       gj_u64_dec_sat(0) == 0; otherwise returns x - 1.
 *   uint64_t __gj_u64_dec_sat  (alias)
 *   __libcgj_batch656_marker = "libcgj-batch656"
 *
 * Distinct from gj_sat_sub_u64 / __gj_sat_sub_u64 (batch377) — different
 * symbol names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch656_marker[] = "libcgj-batch656";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_dec_sat — unsigned decrement; clamp to 0 if x is already 0.
 *
 * Does not wrap: UINT64_MAX decrements to UINT64_MAX - 1; only 0 saturates.
 */
uint64_t
gj_u64_dec_sat(uint64_t x)
{
	if (x == 0u) {
		return 0u;
	}
	return x - 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_dec_sat(uint64_t x)
    __attribute__((alias("gj_u64_dec_sat")));
