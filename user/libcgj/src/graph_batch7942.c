/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7942: strtod optional sign code stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_sign_code_u_7942(uint32_t ch);
 *     - Return 1 for '+', 2 for '-', else 0 (no sign token).
 *   uint32_t __gj_strtod_sign_code_u_7942  (alias)
 *   __libcgj_batch7942_marker = "libcgj-batch7942"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_sign_code_u_7942 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7942_marker[] = "libcgj-batch7942";

#define B7942_SIGN_PLUS   ((uint32_t)1u)
#define B7942_SIGN_MINUS  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7942_sign_code(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'+') {
		return B7942_SIGN_PLUS;
	}
	if (u32Ch == (uint32_t)'-') {
		return B7942_SIGN_MINUS;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_sign_code_u_7942 - soft sign token code for strtod subject.
 *
 * ch: character code
 *
 * Returns 1 for '+', 2 for '-', else 0. Catalog for optional leading
 * sign; does not call strtod(3). No parent wires.
 */
uint32_t
gj_strtod_sign_code_u_7942(uint32_t u32Ch)
{
	(void)NULL;
	return b7942_sign_code(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_sign_code_u_7942(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_sign_code_u_7942")));
