/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11896: static smoke soft gate (wave 11900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11900(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11900  (alias)
 *   __libcgj_batch11896_marker = "libcgj-batch11896"
 *
 * Milestone 11900 exclusive continuum CREATE-ONLY (11891-11900). Unique
 * gj_smoke_soft_11900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11800 (batch11796), gj_smoke_soft_11700 (batch11696),
 * gj_smoke_soft_11600 (batch11596), and gj_dyn_soft_11900
 * (batch11897 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11896_marker[] = "libcgj-batch11896";

/* Smoke soft lamp for wave 11900 (always PASS). */
#define B11896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11896_soft(void)
{
	return B11896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11900 - report smoke soft lamp for wave 11900.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11900 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11900(void)
{
	(void)NULL;
	return b11896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11900(void)
    __attribute__((alias("gj_smoke_soft_11900")));
