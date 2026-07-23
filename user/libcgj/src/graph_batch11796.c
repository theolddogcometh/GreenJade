/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11796: static smoke soft gate (wave 11800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11800(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11800  (alias)
 *   __libcgj_batch11796_marker = "libcgj-batch11796"
 *
 * Milestone 11800 exclusive continuum CREATE-ONLY (11791-11800). Unique
 * gj_smoke_soft_11800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11700 (batch11696), gj_smoke_soft_11600 (batch11596),
 * gj_smoke_soft_11500 (batch11496), and gj_dyn_soft_11800
 * (batch11797 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11796_marker[] = "libcgj-batch11796";

/* Smoke soft lamp for wave 11800 (always PASS). */
#define B11796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11796_soft(void)
{
	return B11796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11800 - report smoke soft lamp for wave 11800.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11800 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11800(void)
{
	(void)NULL;
	return b11796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11800(void)
    __attribute__((alias("gj_smoke_soft_11800")));
