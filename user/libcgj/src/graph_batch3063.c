/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3063: unsigned Q16.16 → uint32 (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16_to_u32_u(uint32_t q);
 *     - Unsigned Q16.16 → integer, discard fraction (q >> 16).
 *   uint32_t __gj_q16_to_u32_u  (alias)
 *   __libcgj_batch3063_marker = "libcgj-batch3063"
 *
 * Distinct from gj_q16_to_int (batch172) and gj_q16_to_int_floor
 * (batch1474) — exclusive unsigned _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3063_marker[] = "libcgj-batch3063";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_to_u32_u — convert unsigned Q16.16 to a uint32 integer.
 *
 * Drops the fractional 16 bits (logical right shift). No parent wires.
 */
uint32_t
gj_q16_to_u32_u(uint32_t u32Q)
{
	(void)NULL;
	return u32Q >> 16;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16_to_u32_u(uint32_t u32Q)
    __attribute__((alias("gj_q16_to_u32_u")));
