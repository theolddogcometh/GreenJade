/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3103: branch-free u32 select (unsigned form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_select_u(uint32_t cond, uint32_t a, uint32_t b);
 *     - Branch-free: cond != 0 → a, else b. Pure unsigned API.
 *   uint32_t __gj_u32_select_u  (alias)
 *   __libcgj_batch3103_marker = "libcgj-batch3103"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_ct_select_u32 (batch1252) / gj_select_u32 (batch120) /
 * gj_u32_cmov_u (batch3101) — unique gj_u32_select_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3103_marker[] = "libcgj-batch3103";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b. */
static uint32_t
b3103_select(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	uint32_t uC;
	uint32_t uMask;

	uC = (uint32_t)(uCond != 0u);
	uMask = 0u - uC;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_select_u - branch-free select between two uint32_t values.
 *
 * cond != 0 → a; cond == 0 → b. No parent wires.
 */
uint32_t
gj_u32_select_u(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3103_select(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_select_u(uint32_t uCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_select_u")));
