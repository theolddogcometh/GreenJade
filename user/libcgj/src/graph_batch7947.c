/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7947: strtod binary/hex-float exponent stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_is_binexp_u_7947(uint32_t ch);
 *     - Return 1 if ch is 'p' or 'P' (hex-float binary exponent), else 0.
 *   uint32_t __gj_strtod_is_binexp_u_7947  (alias)
 *   __libcgj_batch7947_marker = "libcgj-batch7947"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_is_binexp_u_7947 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7947_marker[] = "libcgj-batch7947";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7947_is_binexp(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'p' || u32Ch == (uint32_t)'P') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_is_binexp_u_7947 - 1 if ch is hex-float exponent p/P.
 *
 * ch: character code
 *
 * Returns 1 for 'p' or 'P' (C99 binary exponent after hex significand),
 * else 0. Soft class only; does not call strtod(3). No parent wires.
 */
uint32_t
gj_strtod_is_binexp_u_7947(uint32_t u32Ch)
{
	(void)NULL;
	return b7947_is_binexp(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_is_binexp_u_7947(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_is_binexp_u_7947")));
