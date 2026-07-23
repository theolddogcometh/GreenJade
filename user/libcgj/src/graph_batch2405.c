/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2405: uint64_t saturating left shift.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_shl_sat(uint64_t x, unsigned n);
 *     - Return x << n for n in 0..63. If n >= 64, return 0 (saturate
 *       the shift count; no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_shl_sat  (alias)
 *   __libcgj_batch2405_marker = "libcgj-batch2405"
 *
 * Milestone 2410 exclusive shift/rotate wave (2401-2410). Unique
 * gj_u64_shl_sat surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2405_marker[] = "libcgj-batch2405";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; n >= 64 saturates to 0. */
static uint64_t
b2405_shl_sat(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		return 0u;
	}
	return u64X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_shl_sat - saturating left shift of a 64-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 64 -> 0
 *
 * Returns x << n for n in 0..63; otherwise 0. Does not call libc.
 */
uint64_t
gj_u64_shl_sat(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b2405_shl_sat(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_shl_sat(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_shl_sat")));
