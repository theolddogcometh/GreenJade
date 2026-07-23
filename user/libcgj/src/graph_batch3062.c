/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3062: uint32 → unsigned Q16.16 (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16_from_u32_u(uint32_t v);
 *     - Integer → unsigned Q16.16 (v << 16). Values with integer part
 *       above 0xFFFF wrap in uint32_t (low 32 bits of the shift product).
 *   uint32_t __gj_q16_from_u32_u  (alias)
 *   __libcgj_batch3062_marker = "libcgj-batch3062"
 *
 * Distinct from gj_q16_from_int (batch172) and gj_q16_from_int_sat
 * (batch1473) — exclusive unsigned _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3062_marker[] = "libcgj-batch3062";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_from_u32_u — convert a uint32 integer to unsigned Q16.16.
 *
 * Scales by 2^16. Callers that need a full-precision integer part should
 * keep v <= 0xFFFF; larger values wrap in the uint32 result.
 * No parent wires.
 */
uint32_t
gj_q16_from_u32_u(uint32_t u32V)
{
	(void)NULL;
	return (uint32_t)((uint64_t)u32V << 16);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16_from_u32_u(uint32_t u32V)
    __attribute__((alias("gj_q16_from_u32_u")));
