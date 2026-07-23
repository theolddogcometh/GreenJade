/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2032: saturating uint64_t subtraction.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sub_sat(uint64_t a, uint64_t b);
 *     — a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_u64_sub_sat  (alias)
 *   __libcgj_batch2032_marker = "libcgj-batch2032"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_sat_sub_u64 (batch377) and gj_u64_sat_sub (batch922) — different
 * public names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2032_marker[] = "libcgj-batch2032";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b (clamp to 0). */
static uint64_t
b2032_sub_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_sat — unsigned subtract; clamp to 0 if a < b (underflow).
 *
 * a: minuend
 * b: subtrahend
 */
uint64_t
gj_u64_sub_sat(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2032_sub_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sub_sat(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sub_sat")));
