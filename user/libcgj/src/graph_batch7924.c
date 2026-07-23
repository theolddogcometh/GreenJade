/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7924: scanf conversion sign-char stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_sign_of_u_7924(uint32_t ch);
 *     - Return 1 for '+', 2 for '-', else 0 (no sign).
 *   uint32_t __gj_scanf_sign_of_u_7924  (alias)
 *   __libcgj_batch7924_marker = "libcgj-batch7924"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_sign_of_u_7924 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7924_marker[] = "libcgj-batch7924";

#define B7924_SIGN_NONE  0u
#define B7924_SIGN_PLUS  1u
#define B7924_SIGN_MINUS 2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7924_sign_of(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'+') {
		return B7924_SIGN_PLUS;
	}
	if (u32Ch == (uint32_t)'-') {
		return B7924_SIGN_MINUS;
	}
	return B7924_SIGN_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_sign_of_u_7924 - soft scanf optional-sign classifier.
 *
 * ch: code unit
 *
 * Returns 1 for '+', 2 for '-', 0 otherwise. Soft pure-data tag for
 * integer conversion paths; does not call scanf(3). No parent wires.
 */
uint32_t
gj_scanf_sign_of_u_7924(uint32_t u32Ch)
{
	(void)NULL;
	return b7924_sign_of(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_sign_of_u_7924(uint32_t u32Ch)
    __attribute__((alias("gj_scanf_sign_of_u_7924")));
