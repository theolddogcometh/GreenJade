/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7946: strtod decimal exponent marker stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_is_exp_u_7946(uint32_t ch);
 *     - Return 1 if ch is 'e' or 'E' (decimal exponent marker), else 0.
 *   uint32_t __gj_strtod_is_exp_u_7946  (alias)
 *   __libcgj_batch7946_marker = "libcgj-batch7946"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_is_exp_u_7946 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7946_marker[] = "libcgj-batch7946";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7946_is_exp(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'e' || u32Ch == (uint32_t)'E') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_is_exp_u_7946 - 1 if ch is decimal exponent marker e/E.
 *
 * ch: character code
 *
 * Returns 1 for 'e' or 'E', else 0. Soft class only; does not call
 * strtod(3). No parent wires.
 */
uint32_t
gj_strtod_is_exp_u_7946(uint32_t u32Ch)
{
	(void)NULL;
	return b7946_is_exp(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_is_exp_u_7946(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_is_exp_u_7946")));
