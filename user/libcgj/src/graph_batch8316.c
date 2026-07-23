/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8316: atoi decimal digit value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_atoi_digit_u_8316(uint32_t c);
 *     - '0'..'9' → 0..9; otherwise 0xffffffff (not an atoi digit).
 *   uint32_t __gj_atoi_digit_u_8316  (alias)
 *   __libcgj_batch8316_marker = "libcgj-batch8316"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_atoi_digit_u_8316 surface
 * only; no multi-def. Distinct from gj_digit_val_u_8311 (shared theme,
 * separate symbol). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8316_marker[] = "libcgj-batch8316";

/* Invalid atoi digit sentinel. */
#define B8316_BAD ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

/* atoi is decimal-only: '0'..'9' → value. */
static uint32_t
b8316_atoi_digit(uint32_t u32C)
{
	if (u32C >= (uint32_t)'0' && u32C <= (uint32_t)'9')
		return u32C - (uint32_t)'0';
	return B8316_BAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_atoi_digit_u_8316 - decimal digit value for atoi-style parse.
 *
 * c: character code
 *
 * Returns 0..9 for '0'..'9', else 0xffffffff. Soft catalog step for
 * atoi digit accumulation; does not call atoi(3). No parent wires.
 */
uint32_t
gj_atoi_digit_u_8316(uint32_t u32C)
{
	(void)NULL;
	return b8316_atoi_digit(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_atoi_digit_u_8316(uint32_t u32C)
    __attribute__((alias("gj_atoi_digit_u_8316")));
