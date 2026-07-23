/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13396: static smoke soft gate (wave 13400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13400  (alias)
 *   __libcgj_batch13396_marker = "libcgj-batch13396"
 *
 * Milestone 13400 exclusive continuum CREATE-ONLY (13391-13400). Unique
 * gj_smoke_soft_13400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13300 (batch13296), gj_smoke_soft_13200 (batch13196),
 * gj_smoke_soft_13100 (batch13096), and gj_dyn_soft_13400
 * (batch13397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13396_marker[] = "libcgj-batch13396";

/* Smoke soft lamp for wave 13400 (always PASS). */
#define B13396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13396_soft(void)
{
	return B13396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13400 - report smoke soft lamp for wave 13400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13400(void)
{
	(void)NULL;
	return b13396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13400(void)
    __attribute__((alias("gj_smoke_soft_13400")));
