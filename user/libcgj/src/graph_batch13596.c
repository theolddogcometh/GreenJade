/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13596: static smoke soft gate (wave 13600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13600  (alias)
 *   __libcgj_batch13596_marker = "libcgj-batch13596"
 *
 * Milestone 13600 exclusive continuum CREATE-ONLY (13591-13600). Unique
 * gj_smoke_soft_13600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13500 (batch13496), gj_smoke_soft_13400 (batch13396),
 * gj_smoke_soft_13300 (batch13296), and gj_dyn_soft_13600
 * (batch13597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13596_marker[] = "libcgj-batch13596";

/* Smoke soft lamp for wave 13600 (always PASS). */
#define B13596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13596_soft(void)
{
	return B13596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13600 - report smoke soft lamp for wave 13600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13600(void)
{
	(void)NULL;
	return b13596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13600(void)
    __attribute__((alias("gj_smoke_soft_13600")));
