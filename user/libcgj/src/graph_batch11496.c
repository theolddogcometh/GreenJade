/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11496: static smoke soft gate (wave 11500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11500  (alias)
 *   __libcgj_batch11496_marker = "libcgj-batch11496"
 *
 * Milestone 11500 exclusive continuum CREATE-ONLY (11491-11500). Unique
 * gj_smoke_soft_11500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11400 (batch11396), gj_smoke_soft_11300 (batch11296),
 * gj_smoke_soft_11200 (batch11196), and gj_dyn_soft_11500
 * (batch11497 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11496_marker[] = "libcgj-batch11496";

/* Smoke soft lamp for wave 11500 (always PASS). */
#define B11496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11496_soft(void)
{
	return B11496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11500 - report smoke soft lamp for wave 11500.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11500 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11500(void)
{
	(void)NULL;
	return b11496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11500(void)
    __attribute__((alias("gj_smoke_soft_11500")));
