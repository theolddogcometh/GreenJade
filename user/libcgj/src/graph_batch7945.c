/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7945: strtod hex-float 'x'/'X' marker stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_is_x_marker_u_7945(uint32_t ch);
 *     - Return 1 if ch is 'x' or 'X' (0x hex-float prefix tail), else 0.
 *   uint32_t __gj_strtod_is_x_marker_u_7945  (alias)
 *   __libcgj_batch7945_marker = "libcgj-batch7945"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_is_x_marker_u_7945 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7945_marker[] = "libcgj-batch7945";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7945_is_x_marker(uint32_t u32Ch)
{
	if (u32Ch == (uint32_t)'x' || u32Ch == (uint32_t)'X') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_is_x_marker_u_7945 - 1 if ch is hex-float 'x'/'X' marker.
 *
 * ch: character code (expected after a leading '0' in subject)
 *
 * Returns 1 for 'x' or 'X', else 0. Soft class for C99 0x hex floats;
 * does not call strtod(3). No parent wires.
 */
uint32_t
gj_strtod_is_x_marker_u_7945(uint32_t u32Ch)
{
	(void)NULL;
	return b7945_is_x_marker(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_is_x_marker_u_7945(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_is_x_marker_u_7945")));
