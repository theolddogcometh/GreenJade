/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11196: static smoke soft gate (wave 11200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11200(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11200  (alias)
 *   __libcgj_batch11196_marker = "libcgj-batch11196"
 *
 * Milestone 11200 exclusive continuum CREATE-ONLY (11191-11200). Unique
 * gj_smoke_soft_11200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11100 (batch11096), gj_smoke_soft_11000 (batch10996),
 * gj_smoke_soft_10900 (batch10896), and gj_dyn_soft_11200
 * (batch11197 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11196_marker[] = "libcgj-batch11196";

/* Smoke soft lamp for wave 11200 (always PASS). */
#define B11196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11196_soft(void)
{
	return B11196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11200 - report smoke soft lamp for wave 11200.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11200 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11200(void)
{
	(void)NULL;
	return b11196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11200(void)
    __attribute__((alias("gj_smoke_soft_11200")));
