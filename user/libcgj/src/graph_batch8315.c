/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8315: parse accumulate overflow probe stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_parse_overflow_u_8315(uint32_t acc, uint32_t base,
 *                                     uint32_t digit);
 *     - Return 1 if acc*base + digit would overflow uint32_t; else 0.
 *   uint32_t __gj_parse_overflow_u_8315  (alias)
 *   __libcgj_batch8315_marker = "libcgj-batch8315"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_parse_overflow_u_8315
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8315_marker[] = "libcgj-batch8315";

/* Soft 32-bit ceiling for overflow arithmetic. */
#define B8315_U32_MAX ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Probe: would (acc * base + digit) wrap uint32_t?
 * base==0 is treated as non-overflowing identity path (no mul).
 */
static uint32_t
b8315_parse_overflow(uint32_t u32Acc, uint32_t u32Base, uint32_t u32Digit)
{
	uint32_t u32Lim;
	uint32_t u32Rem;

	if (u32Base <= 1u)
		return 0u;

	u32Lim = B8315_U32_MAX / u32Base;
	if (u32Acc > u32Lim)
		return 1u;

	if (u32Acc == u32Lim) {
		u32Rem = B8315_U32_MAX % u32Base;
		if (u32Digit > u32Rem)
			return 1u;
	}

	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_overflow_u_8315 - 1 if next digit would overflow uint32_t.
 *
 * acc:   current accumulator
 * base:  conversion base (>=2 for meaningful mul)
 * digit: next digit value in [0, base)
 *
 * Returns 1 when acc*base+digit would wrap, else 0. Soft catalog
 * overflow gate for atoi/strtol paths; no libc, no __int128.
 * No parent wires.
 */
uint32_t
gj_parse_overflow_u_8315(uint32_t u32Acc, uint32_t u32Base, uint32_t u32Digit)
{
	(void)NULL;
	return b8315_parse_overflow(u32Acc, u32Base, u32Digit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_parse_overflow_u_8315(uint32_t u32Acc, uint32_t u32Base,
    uint32_t u32Digit)
    __attribute__((alias("gj_parse_overflow_u_8315")));
