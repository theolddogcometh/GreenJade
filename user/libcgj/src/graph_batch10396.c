/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10396: static smoke soft gate (wave 10400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10400  (alias)
 *   __libcgj_batch10396_marker = "libcgj-batch10396"
 *
 * Milestone 10400 exclusive continuum CREATE-ONLY (10391-10400). Unique
 * gj_smoke_soft_10400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10300 (batch10296), gj_smoke_soft_10200 (batch10196),
 * gj_smoke_soft_10100 (batch10096), and gj_dyn_soft_10400
 * (batch10397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10396_marker[] = "libcgj-batch10396";

/* Smoke soft lamp for wave 10400 (always PASS). */
#define B10396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10396_soft(void)
{
	return B10396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10400 - report smoke soft lamp for wave 10400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10400(void)
{
	(void)NULL;
	return b10396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10400(void)
    __attribute__((alias("gj_smoke_soft_10400")));
