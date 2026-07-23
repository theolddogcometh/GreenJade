/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10896: static smoke soft gate (wave 10900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10900(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10900  (alias)
 *   __libcgj_batch10896_marker = "libcgj-batch10896"
 *
 * Milestone 10900 exclusive continuum CREATE-ONLY (10891-10900). Unique
 * gj_smoke_soft_10900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10800 (batch10796), gj_smoke_soft_10700 (batch10696),
 * gj_smoke_soft_10600 (batch10596), and gj_dyn_soft_10900
 * (batch10897 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10896_marker[] = "libcgj-batch10896";

/* Smoke soft lamp for wave 10900 (always PASS). */
#define B10896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10896_soft(void)
{
	return B10896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10900 - report smoke soft lamp for wave 10900.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10900 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10900(void)
{
	(void)NULL;
	return b10896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10900(void)
    __attribute__((alias("gj_smoke_soft_10900")));
