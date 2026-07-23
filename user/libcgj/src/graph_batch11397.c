/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11397: dyn smoke soft gate (wave 11400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_11400(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 11400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_11400  (alias)
 *   __libcgj_batch11397_marker = "libcgj-batch11397"
 *
 * Milestone 11400 exclusive continuum CREATE-ONLY (11391-11400). Unique
 * gj_dyn_soft_11400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_11300 (batch11297), gj_dyn_soft_11200 (batch11197),
 * gj_dyn_soft_11100 (batch11097), and gj_smoke_soft_11400
 * (batch11396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11397_marker[] = "libcgj-batch11397";

/* Dyn soft lamp for wave 11400 (always soft-ready). */
#define B11397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11397_soft(void)
{
	return B11397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_11400 - report dyn soft lamp for wave 11400.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch11396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_11400(void)
{
	(void)NULL;
	return b11397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_11400(void)
    __attribute__((alias("gj_dyn_soft_11400")));
