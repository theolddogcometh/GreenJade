/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3404: branch-free u64 cmovnz (unsigned form).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_cmovnz_u(uint32_t cond, uint64_t a, uint64_t b);
 *     - Branch-free: cond != 0 → a, else b. Pure unsigned API.
 *   uint64_t __gj_u64_cmovnz_u  (alias)
 *   __libcgj_batch3404_marker = "libcgj-batch3404"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_u64_cmov_u (batch3102) / gj_u64_cmovz_u (batch3402) —
 * unique gj_u64_cmovnz_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3404_marker[] = "libcgj-batch3404";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b. */
static uint64_t
b3404_cmovnz(uint32_t uCond, uint64_t uA, uint64_t uB)
{
	uint64_t uNz;
	uint64_t uMask;

	uNz = (uint64_t)(uCond != 0u);
	uMask = 0ull - uNz;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_cmovnz_u - branch-free select when cond is non-zero.
 *
 * cond != 0 → a; cond == 0 → b. No parent wires.
 */
uint64_t
gj_u64_cmovnz_u(uint32_t uCond, uint64_t uA, uint64_t uB)
{
	(void)NULL;
	return b3404_cmovnz(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_cmovnz_u(uint32_t uCond, uint64_t uA, uint64_t uB)
    __attribute__((alias("gj_u64_cmovnz_u")));
