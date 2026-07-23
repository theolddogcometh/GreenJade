/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8318: strtoul base-0 prefix detect stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtoul_prefix_u_8318(uint32_t c0, uint32_t c1);
 *     - Given the first two character codes after optional sign when
 *       base==0: return auto-detected base 16 (0x/0X), 8 (leading 0),
 *       or 10 (default decimal). Unsigned strtoul twin of
 *       gj_strtol_prefix_u_8317.
 *   uint32_t __gj_strtoul_prefix_u_8318  (alias)
 *   __libcgj_batch8318_marker = "libcgj-batch8318"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_strtoul_prefix_u_8318
 * surface only; no multi-def. Distinct from gj_strtol_prefix_u_8317.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8318_marker[] = "libcgj-batch8318";

#define B8318_BASE_DEC 10u
#define B8318_BASE_OCT 8u
#define B8318_BASE_HEX 16u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ISO C strtoul base-0 prefix (same rules as strtol):
 *   "0x"/"0X" → base 16
 *   leading 0 → base 8
 *   otherwise → base 10
 */
static uint32_t
b8318_strtoul_prefix(uint32_t u32C0, uint32_t u32C1)
{
	if (u32C0 == (uint32_t)'0') {
		if (u32C1 == (uint32_t)'x' || u32C1 == (uint32_t)'X')
			return B8318_BASE_HEX;
		return B8318_BASE_OCT;
	}
	return B8318_BASE_DEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtoul_prefix_u_8318 - auto-detect base from strtoul base-0 prefix.
 *
 * c0: first character after optional sign
 * c1: second character (may be 0 if end-of-input)
 *
 * Returns 16 for 0x/0X, 8 for leading 0, else 10. Soft catalog prefix
 * class; does not call strtoul(3). No parent wires.
 */
uint32_t
gj_strtoul_prefix_u_8318(uint32_t u32C0, uint32_t u32C1)
{
	(void)NULL;
	return b8318_strtoul_prefix(u32C0, u32C1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtoul_prefix_u_8318(uint32_t u32C0, uint32_t u32C1)
    __attribute__((alias("gj_strtoul_prefix_u_8318")));
