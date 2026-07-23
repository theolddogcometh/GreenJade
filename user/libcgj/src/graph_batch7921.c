/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7921: scanf conversion digit-value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_digit_value_u_7921(uint32_t ch, uint32_t base);
 *     - Map ASCII digit/letter ch to value in base (2..36).
 *       Invalid digit or base → 0xFFFFFFFFu.
 *   uint32_t __gj_scanf_digit_value_u_7921  (alias)
 *   __libcgj_batch7921_marker = "libcgj-batch7921"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_digit_value_u_7921 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7921_marker[] = "libcgj-batch7921";

#define B7921_INVALID  ((uint32_t)0xFFFFFFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7921_digit_value(uint32_t u32Ch, uint32_t u32Base)
{
	uint32_t u32Val;

	if (u32Base < 2u || u32Base > 36u) {
		return B7921_INVALID;
	}
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		u32Val = u32Ch - (uint32_t)'0';
	} else if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'z') {
		u32Val = u32Ch - (uint32_t)'a' + 10u;
	} else if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'Z') {
		u32Val = u32Ch - (uint32_t)'A' + 10u;
	} else {
		return B7921_INVALID;
	}
	if (u32Val >= u32Base) {
		return B7921_INVALID;
	}
	return u32Val;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_digit_value_u_7921 - soft scanf digit value conversion.
 *
 * ch:   code unit (ASCII digit or A-Z/a-z)
 * base: radix in [2, 36]
 *
 * Returns digit value in [0, base), or 0xFFFFFFFFu when invalid.
 * Pure integer conversion catalog; does not call scanf(3). No parent wires.
 */
uint32_t
gj_scanf_digit_value_u_7921(uint32_t u32Ch, uint32_t u32Base)
{
	(void)NULL;
	return b7921_digit_value(u32Ch, u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_digit_value_u_7921(uint32_t u32Ch, uint32_t u32Base)
    __attribute__((alias("gj_scanf_digit_value_u_7921")));
