/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13696: static smoke soft gate (wave 13700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13700(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13700  (alias)
 *   __libcgj_batch13696_marker = "libcgj-batch13696"
 *
 * Milestone 13700 exclusive continuum CREATE-ONLY (13691-13700). Unique
 * gj_smoke_soft_13700 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13600 (batch13596), gj_smoke_soft_13500 (batch13496),
 * gj_smoke_soft_13400 (batch13396), and gj_dyn_soft_13700
 * (batch13697 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13696_marker[] = "libcgj-batch13696";

/* Smoke soft lamp for wave 13700 (always PASS). */
#define B13696_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13696_soft(void)
{
	return B13696_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13700 - report smoke soft lamp for wave 13700.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13700 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13700(void)
{
	(void)NULL;
	return b13696_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13700(void)
    __attribute__((alias("gj_smoke_soft_13700")));
