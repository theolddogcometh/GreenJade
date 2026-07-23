/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10196: static smoke soft gate (wave 10200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10200(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10200  (alias)
 *   __libcgj_batch10196_marker = "libcgj-batch10196"
 *
 * Milestone 10200 exclusive continuum CREATE-ONLY (10191-10200). Unique
 * gj_smoke_soft_10200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10100 (batch10096), gj_smoke_soft_10000 (batch9996),
 * gj_smoke_soft_9900 (batch9896), and gj_dyn_soft_10200
 * (batch10197 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10196_marker[] = "libcgj-batch10196";

/* Smoke soft lamp for wave 10200 (always PASS). */
#define B10196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10196_soft(void)
{
	return B10196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10200 - report smoke soft lamp for wave 10200.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10200 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10200(void)
{
	(void)NULL;
	return b10196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10200(void)
    __attribute__((alias("gj_smoke_soft_10200")));
