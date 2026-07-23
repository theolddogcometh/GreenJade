/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8319: atoi/strtol whitespace skip class stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_parse_ws_skip_u_8319(uint32_t c);
 *     - Return 1 if c is a leading whitespace character that
 *       atoi/strtol would skip (space, HT, LF, VT, FF, CR); else 0.
 *   uint32_t __gj_parse_ws_skip_u_8319  (alias)
 *   __libcgj_batch8319_marker = "libcgj-batch8319"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_parse_ws_skip_u_8319
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8319_marker[] = "libcgj-batch8319";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * C locale isspace set used by strtol leading skip (ASCII only):
 *   ' '  (0x20), '\t' (0x09), '\n' (0x0a),
 *   '\v' (0x0b), '\f' (0x0c), '\r' (0x0d)
 */
static uint32_t
b8319_parse_ws_skip(uint32_t u32C)
{
	if (u32C == (uint32_t)' ')
		return 1u;
	if (u32C == (uint32_t)'\t')
		return 1u;
	if (u32C == (uint32_t)'\n')
		return 1u;
	if (u32C == (uint32_t)'\v')
		return 1u;
	if (u32C == (uint32_t)'\f')
		return 1u;
	if (u32C == (uint32_t)'\r')
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_ws_skip_u_8319 - 1 if character is skippable leading WS.
 *
 * c: character code
 *
 * Returns 1 for space/HT/LF/VT/FF/CR, else 0. Soft catalog class for
 * atoi/strtol leading whitespace; does not call isspace(3).
 * No parent wires.
 */
uint32_t
gj_parse_ws_skip_u_8319(uint32_t u32C)
{
	(void)NULL;
	return b8319_parse_ws_skip(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_parse_ws_skip_u_8319(uint32_t u32C)
    __attribute__((alias("gj_parse_ws_skip_u_8319")));
