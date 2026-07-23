/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7944: strtod hex digit value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_hexdigit_val_u_7944(uint32_t ch);
 *     - Return 0..15 for hex digits, else 0xFF (not a hex digit).
 *   uint32_t __gj_strtod_hexdigit_val_u_7944  (alias)
 *   __libcgj_batch7944_marker = "libcgj-batch7944"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_hexdigit_val_u_7944 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7944_marker[] = "libcgj-batch7944";

#define B7944_NOT_HEX  ((uint32_t)0xFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7944_hexdigit_val(uint32_t u32Ch)
{
	if (u32Ch >= (uint32_t)'0' && u32Ch <= (uint32_t)'9') {
		return u32Ch - (uint32_t)'0';
	}
	if (u32Ch >= (uint32_t)'a' && u32Ch <= (uint32_t)'f') {
		return (u32Ch - (uint32_t)'a') + 10u;
	}
	if (u32Ch >= (uint32_t)'A' && u32Ch <= (uint32_t)'F') {
		return (u32Ch - (uint32_t)'A') + 10u;
	}
	return B7944_NOT_HEX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_hexdigit_val_u_7944 - soft hex digit value (C99 hex float).
 *
 * ch: character code
 *
 * Returns 0..15 for 0-9/a-f/A-F, else 0xFF. Soft class only; does not
 * call strtod(3). No parent wires.
 */
uint32_t
gj_strtod_hexdigit_val_u_7944(uint32_t u32Ch)
{
	(void)NULL;
	return b7944_hexdigit_val(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_hexdigit_val_u_7944(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_hexdigit_val_u_7944")));
