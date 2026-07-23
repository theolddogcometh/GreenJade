/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7913: printf format space flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_flag_space_bit_7913(void);
 *     - Returns soft FLAG_SPACE bit for printf ' ' (always 0x04).
 *   uint32_t __gj_printf_flag_space_bit_7913  (alias)
 *   __libcgj_batch7913_marker = "libcgj-batch7913"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique gj_printf_flag_space_bit_7913 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7913_marker[] = "libcgj-batch7913";

/* Soft FLAG_SPACE (' ') bit in a printf flags word. */
#define B7913_FLAG_SPACE  ((uint32_t)0x04u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7913_space_bit(void)
{
	return B7913_FLAG_SPACE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_flag_space_bit_7913 - soft FLAG_SPACE bit for printf ' '.
 *
 * Always returns 0x04. Catalog constant for positive-space flag packing.
 * Does not parse formats or call printf. No parent wires.
 */
uint32_t
gj_printf_flag_space_bit_7913(void)
{
	(void)NULL;
	return b7913_space_bit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printf_flag_space_bit_7913(void)
    __attribute__((alias("gj_printf_flag_space_bit_7913")));
