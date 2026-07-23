/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8314: strtol base validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_parse_base_ok_u_8314(uint32_t base);
 *     - Return 1 if base is 0 (auto) or in 2..36; else 0.
 *   uint32_t __gj_parse_base_ok_u_8314  (alias)
 *   __libcgj_batch8314_marker = "libcgj-batch8314"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_parse_base_ok_u_8314
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8314_marker[] = "libcgj-batch8314";

/* ISO C strtol base range: 0 or 2..36. */
#define B8314_BASE_MIN 2u
#define B8314_BASE_MAX 36u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8314_parse_base_ok(uint32_t u32Base)
{
	if (u32Base == 0u)
		return 1u;
	if (u32Base >= B8314_BASE_MIN && u32Base <= B8314_BASE_MAX)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_base_ok_u_8314 - 1 if base is legal for strtol-style parse.
 *
 * base: requested conversion base (0 = auto-detect)
 *
 * Returns 1 when base is 0 or in [2, 36], else 0. Soft catalog gate;
 * does not call strtol(3). No parent wires.
 */
uint32_t
gj_parse_base_ok_u_8314(uint32_t u32Base)
{
	(void)NULL;
	return b8314_parse_base_ok(u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_parse_base_ok_u_8314(uint32_t u32Base)
    __attribute__((alias("gj_parse_base_ok_u_8314")));
