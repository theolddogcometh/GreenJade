/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3403: branch-free u32 cmovnz (unsigned form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cmovnz_u(uint32_t cond, uint32_t a, uint32_t b);
 *     - Branch-free: cond != 0 → a, else b. Pure unsigned API.
 *   uint32_t __gj_u32_cmovnz_u  (alias)
 *   __libcgj_batch3403_marker = "libcgj-batch3403"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_u32_cmov_u (batch3101) / gj_u32_cmovz_u (batch3401) —
 * unique gj_u32_cmovnz_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3403_marker[] = "libcgj-batch3403";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b. */
static uint32_t
b3403_cmovnz(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	uint32_t uNz;
	uint32_t uMask;

	uNz = (uint32_t)(uCond != 0u);
	uMask = 0u - uNz;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cmovnz_u - branch-free select when cond is non-zero.
 *
 * cond != 0 → a; cond == 0 → b. Mask derived without a data-dependent
 * branch on a/b. No parent wires.
 */
uint32_t
gj_u32_cmovnz_u(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3403_cmovnz(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cmovnz_u(uint32_t uCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_cmovnz_u")));
