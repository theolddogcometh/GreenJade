/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7915: printf format zero-pad flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_zero_bit_7915(void);
 *     - Returns soft FLAG_ZERO bit for printf '0' (always 0x10).
 *   uint32_t __gj_printf_flag_zero_bit_7915  (alias)
 *   __libcgj_batch7915_marker = "libcgj-batch7915"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_zero_bit_7915 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7915_marker[] = "libcgj-batch7915";

/* Soft FLAG_ZERO ('0') bit in a printf flags word. */
#define B7915_FLAG_ZERO  ((uint32_t)0x10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7915_zero_bit(void)
{
	return B7915_FLAG_ZERO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_zero_bit_7915 - soft FLAG_ZERO bit for printf '0'.
 *
 * Always returns 0x10. Catalog constant for zero-pad flag packing.
 * Does not parse formats or call printf. No parent wires.
 */
uint32_t
gj_printf_flag_zero_bit_7915(void)
{
	(void)NULL;
	return b7915_zero_bit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_zero_bit_7915(void)
    __attribute__((alias("gj_printf_flag_zero_bit_7915")));
