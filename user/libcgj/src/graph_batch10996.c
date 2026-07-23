/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10996: static smoke soft gate (wave 11000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_11000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 11000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_11000  (alias)
 *   __libcgj_batch10996_marker = "libcgj-batch10996"
 *
 * Milestone 11000 exclusive continuum CREATE-ONLY (10991-11000). Unique
 * gj_smoke_soft_11000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10900 (batch10896), gj_smoke_soft_10800 (batch10796),
 * gj_smoke_soft_10700 (batch10696), and gj_dyn_soft_11000
 * (batch10997 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10996_marker[] = "libcgj-batch10996";

/* Smoke soft lamp for wave 11000 (always PASS). */
#define B10996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10996_soft(void)
{
	return B10996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_11000 - report smoke soft lamp for wave 11000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_11000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_11000(void)
{
	(void)NULL;
	return b10996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_11000(void)
    __attribute__((alias("gj_smoke_soft_11000")));
