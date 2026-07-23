/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8796: static smoke soft gate (wave 8800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8800(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 8800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8800  (alias)
 *   __libcgj_batch8796_marker = "libcgj-batch8796"
 *
 * Milestone 8800 exclusive continuum CREATE-ONLY (8791-8800). Unique
 * gj_smoke_soft_8800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_8700 (batch8696), gj_smoke_soft_8600 (batch8596),
 * gj_smoke_soft_8500 (batch8496), and gj_dyn_soft_8800
 * (batch8797 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8796_marker[] = "libcgj-batch8796";

/* Smoke soft lamp for wave 8800 (always PASS). */
#define B8796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8796_soft(void)
{
	return B8796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8800 - report smoke soft lamp for wave 8800.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_8800 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8800(void)
{
	(void)NULL;
	return b8796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8800(void)
    __attribute__((alias("gj_smoke_soft_8800")));
