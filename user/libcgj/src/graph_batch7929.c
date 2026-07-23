/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7929: scanf conversion digit-accumulate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_accum_digit_u_7929(uint32_t cur, uint32_t base,
 *                                        uint32_t digit);
 *     - Soft cur*base + digit with overflow → 0xFFFFFFFFu.
 *       Rejects base outside [2,36] or digit >= base with 0xFFFFFFFFu.
 *   uint32_t __gj_scanf_accum_digit_u_7929  (alias)
 *   __libcgj_batch7929_marker = "libcgj-batch7929"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_accum_digit_u_7929 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7929_marker[] = "libcgj-batch7929";

#define B7929_OVERFLOW  ((uint32_t)0xFFFFFFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7929_accum_digit(uint32_t u32Cur, uint32_t u32Base, uint32_t u32Digit)
{
	uint32_t u32Prod;
	uint32_t u32Sum;

	if (u32Base < 2u || u32Base > 36u) {
		return B7929_OVERFLOW;
	}
	if (u32Digit >= u32Base) {
		return B7929_OVERFLOW;
	}
	/* Overflow-safe mul: cur * base. */
	if (u32Cur > (UINT32_MAX / u32Base)) {
		return B7929_OVERFLOW;
	}
	u32Prod = u32Cur * u32Base;
	if (u32Prod > (UINT32_MAX - u32Digit)) {
		return B7929_OVERFLOW;
	}
	u32Sum = u32Prod + u32Digit;
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_accum_digit_u_7929 - soft scanf integer digit accumulate.
 *
 * cur:   running unsigned value
 * base:  radix in [2, 36]
 * digit: next digit value in [0, base)
 *
 * Returns cur*base + digit, or 0xFFFFFFFFu on bad args / overflow.
 * Pure integer path; no __int128, no scanf(3). No parent wires.
 */
uint32_t
gj_scanf_accum_digit_u_7929(uint32_t u32Cur, uint32_t u32Base,
                            uint32_t u32Digit)
{
	(void)NULL;
	return b7929_accum_digit(u32Cur, u32Base, u32Digit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_accum_digit_u_7929(uint32_t u32Cur, uint32_t u32Base,
                                       uint32_t u32Digit)
    __attribute__((alias("gj_scanf_accum_digit_u_7929")));
