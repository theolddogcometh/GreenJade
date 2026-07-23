/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11497: dyn smoke soft gate (wave 11500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_11500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 11500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_11500  (alias)
 *   __libcgj_batch11497_marker = "libcgj-batch11497"
 *
 * Milestone 11500 exclusive continuum CREATE-ONLY (11491-11500). Unique
 * gj_dyn_soft_11500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_11400 (batch11397), gj_dyn_soft_11300 (batch11297),
 * gj_dyn_soft_11200 (batch11197), and gj_smoke_soft_11500
 * (batch11496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11497_marker[] = "libcgj-batch11497";

/* Dyn soft lamp for wave 11500 (always soft-ready). */
#define B11497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11497_soft(void)
{
	return B11497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_11500 - report dyn soft lamp for wave 11500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch11496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_11500(void)
{
	(void)NULL;
	return b11497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_11500(void)
    __attribute__((alias("gj_dyn_soft_11500")));
