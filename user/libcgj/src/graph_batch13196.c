/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13196: static smoke soft gate (wave 13200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13200(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13200  (alias)
 *   __libcgj_batch13196_marker = "libcgj-batch13196"
 *
 * Milestone 13200 exclusive continuum CREATE-ONLY (13191-13200). Unique
 * gj_smoke_soft_13200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13100 (batch13096), gj_smoke_soft_13000 (batch12996),
 * gj_smoke_soft_12900 (batch12896), and gj_dyn_soft_13200
 * (batch13197 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13196_marker[] = "libcgj-batch13196";

/* Smoke soft lamp for wave 13200 (always PASS). */
#define B13196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13196_soft(void)
{
	return B13196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13200 - report smoke soft lamp for wave 13200.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13200 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13200(void)
{
	(void)NULL;
	return b13196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13200(void)
    __attribute__((alias("gj_smoke_soft_13200")));
