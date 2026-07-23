/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8313: atoi/strtol optional sign class stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_parse_sign_u_8313(uint32_t c);
 *     - '+' → 1 (positive sign present); '-' → 2 (negative);
 *       otherwise → 0 (no sign character).
 *   uint32_t __gj_parse_sign_u_8313  (alias)
 *   __libcgj_batch8313_marker = "libcgj-batch8313"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_parse_sign_u_8313 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8313_marker[] = "libcgj-batch8313";

/* Sign class codes for optional leading sign. */
#define B8313_NONE  0u
#define B8313_PLUS  1u
#define B8313_MINUS 2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8313_parse_sign(uint32_t u32C)
{
	if (u32C == (uint32_t)'+')
		return B8313_PLUS;
	if (u32C == (uint32_t)'-')
		return B8313_MINUS;
	return B8313_NONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_sign_u_8313 - classify optional leading sign character.
 *
 * c: character code after whitespace skip
 *
 * Returns 1 for '+', 2 for '-', else 0. Soft catalog class for
 * atoi/strtol sign handling; does not call libc. No parent wires.
 */
uint32_t
gj_parse_sign_u_8313(uint32_t u32C)
{
	(void)NULL;
	return b8313_parse_sign(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_parse_sign_u_8313(uint32_t u32C)
    __attribute__((alias("gj_parse_sign_u_8313")));
