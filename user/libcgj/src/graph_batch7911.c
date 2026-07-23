/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7911: printf format left-justify flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_left_bit_7911(void);
 *     - Returns soft FLAG_LEFT bit for printf '-' (always 0x01).
 *   uint32_t __gj_printf_flag_left_bit_7911  (alias)
 *   __libcgj_batch7911_marker = "libcgj-batch7911"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_left_bit_7911 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7911_marker[] = "libcgj-batch7911";

/* Soft FLAG_LEFT ('-') bit in a printf flags word. */
#define B7911_FLAG_LEFT  ((uint32_t)0x01u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7911_left_bit(void)
{
	return B7911_FLAG_LEFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_left_bit_7911 - soft FLAG_LEFT bit for printf '-'.
 *
 * Always returns 0x01. Catalog constant for left-justify flag packing.
 * Does not parse formats or call printf. No parent wires.
 */
uint32_t
gj_printf_flag_left_bit_7911(void)
{
	(void)NULL;
	return b7911_left_bit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_left_bit_7911(void)
    __attribute__((alias("gj_printf_flag_left_bit_7911")));
