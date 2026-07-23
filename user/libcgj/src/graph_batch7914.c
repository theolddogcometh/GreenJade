/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7914: printf format alternate-form flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_alt_bit_7914(void);
 *     - Returns soft FLAG_ALT bit for printf '#' (always 0x08).
 *   uint32_t __gj_printf_flag_alt_bit_7914  (alias)
 *   __libcgj_batch7914_marker = "libcgj-batch7914"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_alt_bit_7914 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7914_marker[] = "libcgj-batch7914";

/* Soft FLAG_ALT ('#') bit in a printf flags word. */
#define B7914_FLAG_ALT  ((uint32_t)0x08u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7914_alt_bit(void)
{
	return B7914_FLAG_ALT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_alt_bit_7914 - soft FLAG_ALT bit for printf '#'.
 *
 * Always returns 0x08. Catalog constant for alternate-form flag packing.
 * Does not parse formats or call printf. No parent wires.
 */
uint32_t
gj_printf_flag_alt_bit_7914(void)
{
	(void)NULL;
	return b7914_alt_bit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_alt_bit_7914(void)
    __attribute__((alias("gj_printf_flag_alt_bit_7914")));
