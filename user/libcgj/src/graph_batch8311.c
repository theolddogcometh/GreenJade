/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8311: atoi/strtol decimal digit value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_digit_val_u_8311(uint32_t c);
 *     - '0'..'9' → 0..9; otherwise 0xffffffff (invalid).
 *   uint32_t __gj_digit_val_u_8311  (alias)
 *   __libcgj_batch8311_marker = "libcgj-batch8311"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_digit_val_u_8311 surface
 * only; no multi-def. Distinct from gj_digit_val (batch828). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8311_marker[] = "libcgj-batch8311";

/* Invalid digit sentinel (not in 0..9). */
#define B8311_BAD ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8311_digit_val(uint32_t u32C)
{
	if (u32C >= (uint32_t)'0' && u32C <= (uint32_t)'9')
		return u32C - (uint32_t)'0';
	return B8311_BAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digit_val_u_8311 - decimal value of ASCII digit character.
 *
 * c: character code (integer path; low octet is the ASCII unit)
 *
 * Returns 0..9 for '0'..'9', else 0xffffffff. Soft catalog map for
 * atoi/strtol digit accumulation; does not call libc. No parent wires.
 */
uint32_t
gj_digit_val_u_8311(uint32_t u32C)
{
	(void)NULL;
	return b8311_digit_val(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_digit_val_u_8311(uint32_t u32C)
    __attribute__((alias("gj_digit_val_u_8311")));
