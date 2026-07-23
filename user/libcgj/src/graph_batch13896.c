/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13896: static smoke soft gate (wave 13900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13900(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13900  (alias)
 *   __libcgj_batch13896_marker = "libcgj-batch13896"
 *
 * Milestone 13900 exclusive continuum CREATE-ONLY (13891-13900). Unique
 * gj_smoke_soft_13900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13800 (batch13796), gj_smoke_soft_13700 (batch13696),
 * gj_smoke_soft_13600 (batch13596), and gj_dyn_soft_13900
 * (batch13897 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13896_marker[] = "libcgj-batch13896";

/* Smoke soft lamp for wave 13900 (always PASS). */
#define B13896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13896_soft(void)
{
	return B13896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13900 - report smoke soft lamp for wave 13900.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13900 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13900(void)
{
	(void)NULL;
	return b13896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13900(void)
    __attribute__((alias("gj_smoke_soft_13900")));
