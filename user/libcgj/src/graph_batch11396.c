/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11396: static smoke soft gate (wave 11400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11400  (alias)
 *   __libcgj_batch11396_marker = "libcgj-batch11396"
 *
 * Milestone 11400 exclusive continuum CREATE-ONLY (11391-11400). Unique
 * gj_smoke_soft_11400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11300 (batch11296), gj_smoke_soft_11200 (batch11196),
 * gj_smoke_soft_11100 (batch11096), and gj_dyn_soft_11400
 * (batch11397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11396_marker[] = "libcgj-batch11396";

/* Smoke soft lamp for wave 11400 (always PASS). */
#define B11396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11396_soft(void)
{
	return B11396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11400 - report smoke soft lamp for wave 11400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11400(void)
{
	(void)NULL;
	return b11396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11400(void)
    __attribute__((alias("gj_smoke_soft_11400")));
