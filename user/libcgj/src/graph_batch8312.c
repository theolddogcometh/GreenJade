/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8312: atoi/strtol hex digit value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xdigit_val_u_8312(uint32_t c);
 *     - '0'..'9' → 0..9; 'a'..'f'/'A'..'F' → 10..15; else 0xffffffff.
 *   uint32_t __gj_xdigit_val_u_8312  (alias)
 *   __libcgj_batch8312_marker = "libcgj-batch8312"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Unique gj_xdigit_val_u_8312 surface
 * only; no multi-def. Distinct from gj_ascii_isxdigit_u (batch2817).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8312_marker[] = "libcgj-batch8312";

/* Invalid xdigit sentinel (not in 0..15). */
#define B8312_BAD ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8312_xdigit_val(uint32_t u32C)
{
	if (u32C >= (uint32_t)'0' && u32C <= (uint32_t)'9')
		return u32C - (uint32_t)'0';
	if (u32C >= (uint32_t)'a' && u32C <= (uint32_t)'f')
		return (u32C - (uint32_t)'a') + 10u;
	if (u32C >= (uint32_t)'A' && u32C <= (uint32_t)'F')
		return (u32C - (uint32_t)'A') + 10u;
	return B8312_BAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xdigit_val_u_8312 - value of ASCII hexadecimal digit character.
 *
 * c: character code (integer path)
 *
 * Returns 0..15 for 0-9/a-f/A-F, else 0xffffffff. Soft catalog map for
 * strtol base-16 paths; does not call libc. No parent wires.
 */
uint32_t
gj_xdigit_val_u_8312(uint32_t u32C)
{
	(void)NULL;
	return b8312_xdigit_val(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xdigit_val_u_8312(uint32_t u32C)
    __attribute__((alias("gj_xdigit_val_u_8312")));
