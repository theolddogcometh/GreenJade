/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4425: SIMD-ish scalar u32 select (mask ? a : b).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_select_u2(uint32_t mask, uint32_t a, uint32_t b);
 *     - If mask != 0 return a, else return b. Branch-free bit-mask form.
 *   uint32_t __gj_u32_select_u2  (alias)
 *   __libcgj_batch4425_marker = "libcgj-batch4425"
 *
 * Exclusive continuum CREATE-ONLY (4421-4430: u32_min_u, u32_max_u,
 * u64_min_u, u64_max_u, u32_select_u, u32_cond_assign_u, u8_avg_u,
 * u16_avg_u, u32_midpoint_u, batch_id_4430). SIMD-ish scalar unique
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4425_marker[] = "libcgj-batch4425";

/* ---- freestanding helpers ---------------------------------------------- */

/* mask != 0 → a; else b. Branch-free on a/b via all-ones mask. */
static uint32_t
b4425_select(uint32_t uMask, uint32_t uA, uint32_t uB)
{
	uint32_t uC;
	uint32_t uSel;

	uC = (uint32_t)(uMask != 0u);
	uSel = 0u - uC;
	return (uA & uSel) | (uB & ~uSel);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_select_u2 - select between two uint32_t values by nonzero mask.
 *
 * mask: nonzero selects a; zero selects b
 * a: value when mask != 0
 * b: value when mask == 0
 *
 * Returns a if mask != 0, otherwise b. Self-contained; no parent wires.
 */
uint32_t
gj_u32_select_u2(uint32_t uMask, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b4425_select(uMask, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_select_u2(uint32_t uMask, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_select_u2")));
