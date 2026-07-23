/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7941: strtod leading-whitespace class stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtod_is_ws_u_7941(uint32_t ch);
 *     - Return 1 if ch is strtod leading whitespace
 *       (space, tab, LF, VT, FF, CR), else 0.
 *   uint32_t __gj_strtod_is_ws_u_7941  (alias)
 *   __libcgj_batch7941_marker = "libcgj-batch7941"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Unique gj_strtod_is_ws_u_7941 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7941_marker[] = "libcgj-batch7941";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7941_is_ws(uint32_t u32Ch)
{
	/* C locale isspace subset used by strtod: 0x09..0x0D, 0x20. */
	if (u32Ch == 0x20u) {
		return 1u;
	}
	if (u32Ch >= 0x09u && u32Ch <= 0x0du) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtod_is_ws_u_7941 - 1 if ch is strtod leading whitespace.
 *
 * ch: character code (octet-range values only are meaningful)
 *
 * Returns 1 for space/tab/LF/VT/FF/CR, else 0. Soft char class; does
 * not call strtod(3) or isspace(3). No parent wires.
 */
uint32_t
gj_strtod_is_ws_u_7941(uint32_t u32Ch)
{
	(void)NULL;
	return b7941_is_ws(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtod_is_ws_u_7941(uint32_t u32Ch)
    __attribute__((alias("gj_strtod_is_ws_u_7941")));
