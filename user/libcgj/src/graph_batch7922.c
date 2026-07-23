/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7922: scanf conversion is-digit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_is_digit_u_7922(uint32_t ch, uint32_t base);
 *     - Return 1 if ch is a valid digit in base (2..36), else 0.
 *   uint32_t __gj_scanf_is_digit_u_7922  (alias)
 *   __libcgj_batch7922_marker = "libcgj-batch7922"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_is_digit_u_7922 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7922_marker[] = "libcgj-batch7922";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7922_digit_value(uint32_t u32Ch, uint32_t u32Base)
{
	uint32_t u32Val;

	if (u32Base < 2u || u32Base > 36u) {
		return 0xFFFFFFFFu;
	}
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		u32Val = u32Ch - (uint32_t)'0';
	} else if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'z') {
		u32Val = u32Ch - (uint32_t)'a' + 10u;
	} else if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'Z') {
		u32Val = u32Ch - (uint32_t)'A' + 10u;
	} else {
		return 0xFFFFFFFFu;
	}
	if (u32Val >= u32Base) {
		return 0xFFFFFFFFu;
	}
	return u32Val;
}

static uint32_t
b7922_is_digit(uint32_t u32Ch, uint32_t u32Base)
{
	return (b7922_digit_value(u32Ch, u32Base) != 0xFFFFFFFFu) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_is_digit_u_7922 - soft scanf digit-class predicate.
 *
 * ch:   code unit
 * base: radix in [2, 36]
 *
 * Returns 1 when ch is a valid digit for base, else 0 (including bad base).
 * Pure integer class test; does not call scanf(3). No parent wires.
 */
uint32_t
gj_scanf_is_digit_u_7922(uint32_t u32Ch, uint32_t u32Base)
{
	(void)NULL;
	return b7922_is_digit(u32Ch, u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_is_digit_u_7922(uint32_t u32Ch, uint32_t u32Base)
    __attribute__((alias("gj_scanf_is_digit_u_7922")));
