/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2408: uint32_t saturating logical right shift.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shr_sat(uint32_t x, unsigned n);
 *     - Return x >> n (logical) for n in 0..31. If n >= 32, return 0
 *       (saturate the shift count; no shift UB). Pure integer; no
 *       builtins.
 *   uint32_t __gj_u32_shr_sat  (alias)
 *   __libcgj_batch2408_marker = "libcgj-batch2408"
 *
 * Milestone 2410 exclusive shift/rotate wave (2401-2410). Unique
 * gj_u32_shr_sat surface only; no multi-def. Pair with
 * gj_u32_shl_sat (batch2407).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2408_marker[] = "libcgj-batch2408";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical right shift; n >= 32 saturates to 0. */
static uint32_t
b2408_shr_sat(uint32_t u32X, unsigned uN)
{
	if (uN >= 32u) {
		return 0u;
	}
	return u32X >> uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shr_sat - saturating logical right shift of a 32-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 32 -> 0
 *
 * Returns x >> n for n in 0..31; otherwise 0. Does not call libc.
 */
uint32_t
gj_u32_shr_sat(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b2408_shr_sat(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shr_sat(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shr_sat")));
