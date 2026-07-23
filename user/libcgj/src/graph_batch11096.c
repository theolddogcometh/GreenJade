/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11096: static smoke soft gate (wave 11100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11100(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11100  (alias)
 *   __libcgj_batch11096_marker = "libcgj-batch11096"
 *
 * Milestone 11100 exclusive continuum CREATE-ONLY (11091-11100). Unique
 * gj_smoke_soft_11100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11000 (batch10996), gj_smoke_soft_10900 (batch10896),
 * gj_smoke_soft_10800 (batch10796), and gj_dyn_soft_11100
 * (batch11097 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11096_marker[] = "libcgj-batch11096";

/* Smoke soft lamp for wave 11100 (always PASS). */
#define B11096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11096_soft(void)
{
	return B11096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11100 - report smoke soft lamp for wave 11100.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11100 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11100(void)
{
	(void)NULL;
	return b11096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11100(void)
    __attribute__((alias("gj_smoke_soft_11100")));
