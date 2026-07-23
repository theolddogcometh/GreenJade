/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3102: branch-free u64 cmov (unsigned form).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_cmov_u(uint32_t cond, uint64_t a, uint64_t b);
 *     - Branch-free: cond != 0 → a, else b. Pure unsigned API.
 *   uint64_t __gj_u64_cmov_u  (alias)
 *   __libcgj_batch3102_marker = "libcgj-batch3102"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_ct_cmov_u64 (batch1511) / gj_ct_select_u64 (batch1251) — unique
 * gj_u64_cmov_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3102_marker[] = "libcgj-batch3102";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b. */
static uint64_t
b3102_cmov(uint32_t uCond, uint64_t uA, uint64_t uB)
{
	uint64_t uC;
	uint64_t uMask;

	uC = (uint64_t)(uCond != 0u);
	uMask = 0ull - uC;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_cmov_u - branch-free select between two uint64_t values.
 *
 * cond != 0 → a; cond == 0 → b. No parent wires.
 */
uint64_t
gj_u64_cmov_u(uint32_t uCond, uint64_t uA, uint64_t uB)
{
	(void)NULL;
	return b3102_cmov(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_cmov_u(uint32_t uCond, uint64_t uA, uint64_t uB)
    __attribute__((alias("gj_u64_cmov_u")));
