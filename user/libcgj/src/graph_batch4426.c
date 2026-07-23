/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4426: SIMD-ish scalar u32 cond-assign select.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cond_assign_u(uint32_t cond, uint32_t a, uint32_t b);
 *     - Same semantics as select: cond != 0 → a, else b.
 *   uint32_t __gj_u32_cond_assign_u  (alias)
 *   __libcgj_batch4426_marker = "libcgj-batch4426"
 *
 * Exclusive continuum CREATE-ONLY (4421-4430: u32_min_u, u32_max_u,
 * u64_min_u, u64_max_u, u32_select_u, u32_cond_assign_u, u8_avg_u,
 * u16_avg_u, u32_midpoint_u, batch_id_4430). SIMD-ish scalar unique
 * wave. Unique gj_u32_cond_assign_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4426_marker[] = "libcgj-batch4426";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b via all-ones mask. */
static uint32_t
b4426_cond_assign(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	uint32_t uC;
	uint32_t uSel;

	uC = (uint32_t)(uCond != 0u);
	uSel = 0u - uC;
	return (uA & uSel) | (uB & ~uSel);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cond_assign_u - conditional assign / select of two uint32_t.
 *
 * cond: nonzero selects a; zero selects b
 * a: value when cond != 0
 * b: value when cond == 0
 *
 * Same as select: returns a if cond != 0, otherwise b.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_cond_assign_u(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b4426_cond_assign(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cond_assign_u(uint32_t uCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_cond_assign_u")));
