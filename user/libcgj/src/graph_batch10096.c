/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10096: static smoke soft gate (wave 10100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10100(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10100  (alias)
 *   __libcgj_batch10096_marker = "libcgj-batch10096"
 *
 * Milestone 10100 exclusive continuum CREATE-ONLY (10091-10100). Unique
 * gj_smoke_soft_10100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_9900 (batch9896), gj_smoke_soft_9100 (batch9096),
 * gj_smoke_soft_9000 (batch8996), and gj_dyn_soft_10100
 * (batch10097 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10096_marker[] = "libcgj-batch10096";

/* Smoke soft lamp for wave 10100 (always PASS). */
#define B10096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10096_soft(void)
{
	return B10096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10100 - report smoke soft lamp for wave 10100.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10100 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10100(void)
{
	(void)NULL;
	return b10096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10100(void)
    __attribute__((alias("gj_smoke_soft_10100")));
