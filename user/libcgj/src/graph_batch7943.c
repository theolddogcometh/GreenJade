/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7943: strtod decimal digit value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_digit_val_u_7943(uint32_t ch);
 *     - Return 0..9 for '0'..'9', else 0xFF (not a decimal digit).
 *   uint32_t __gj_strtod_digit_val_u_7943  (alias)
 *   __libcgj_batch7943_marker = "libcgj-batch7943"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_digit_val_u_7943 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7943_marker[] = "libcgj-batch7943";

#define B7943_NOT_DIGIT  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7943_digit_val(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return u32Ch - (uint32_t)'0';
	}
	return B7943_NOT_DIGIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_digit_val_u_7943 - soft decimal digit value for strtod.
 *
 * ch: character code
 *
 * Returns 0..9 for ASCII digits, else 0xFF. Soft class only; does not
 * call strtod(3). No parent wires.
 */
uint32_t
gj_strtod_digit_val_u_7943(uint32_t u32Ch)
{
	(void)NULL;
	return b7943_digit_val(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_digit_val_u_7943(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_digit_val_u_7943")));
