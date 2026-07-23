/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11596: static smoke soft gate (wave 11600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11600  (alias)
 *   __libcgj_batch11596_marker = "libcgj-batch11596"
 *
 * Milestone 11600 exclusive continuum CREATE-ONLY (11591-11600). Unique
 * gj_smoke_soft_11600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11500 (batch11496), gj_smoke_soft_11400 (batch11396),
 * gj_smoke_soft_11300 (batch11296), and gj_dyn_soft_11600
 * (batch11597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11596_marker[] = "libcgj-batch11596";

/* Smoke soft lamp for wave 11600 (always PASS). */
#define B11596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11596_soft(void)
{
	return B11596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11600 - report smoke soft lamp for wave 11600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11600(void)
{
	(void)NULL;
	return b11596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11600(void)
    __attribute__((alias("gj_smoke_soft_11600")));
