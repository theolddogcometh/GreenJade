/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10796: static smoke soft gate (wave 10800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10800(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10800  (alias)
 *   __libcgj_batch10796_marker = "libcgj-batch10796"
 *
 * Milestone 10800 exclusive continuum CREATE-ONLY (10791-10800). Unique
 * gj_smoke_soft_10800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10700 (batch10696), gj_smoke_soft_10600 (batch10596),
 * gj_smoke_soft_10500 (batch10496), and gj_dyn_soft_10800
 * (batch10797 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10796_marker[] = "libcgj-batch10796";

/* Smoke soft lamp for wave 10800 (always PASS). */
#define B10796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10796_soft(void)
{
	return B10796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10800 - report smoke soft lamp for wave 10800.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10800 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10800(void)
{
	(void)NULL;
	return b10796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10800(void)
    __attribute__((alias("gj_smoke_soft_10800")));
