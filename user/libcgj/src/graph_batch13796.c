/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13796: static smoke soft gate (wave 13800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13800(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13800  (alias)
 *   __libcgj_batch13796_marker = "libcgj-batch13796"
 *
 * Milestone 13800 exclusive continuum CREATE-ONLY (13791-13800). Unique
 * gj_smoke_soft_13800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13700 (batch13696), gj_smoke_soft_13600 (batch13596),
 * gj_smoke_soft_13500 (batch13496), and gj_dyn_soft_13800
 * (batch13797 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13796_marker[] = "libcgj-batch13796";

/* Smoke soft lamp for wave 13800 (always PASS). */
#define B13796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13796_soft(void)
{
	return B13796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13800 - report smoke soft lamp for wave 13800.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13800 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13800(void)
{
	(void)NULL;
	return b13796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13800(void)
    __attribute__((alias("gj_smoke_soft_13800")));
