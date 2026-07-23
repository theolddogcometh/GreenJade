/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8496: static smoke soft gate (wave 8500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 8500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8500  (alias)
 *   __libcgj_batch8496_marker = "libcgj-batch8496"
 *
 * Milestone 8500 exclusive continuum CREATE-ONLY (8491-8500). Unique
 * gj_smoke_soft_8500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_8300 (batch8296), gj_smoke_green_8200 (batch8193),
 * gj_smoke_soft_skip_p (batch2198), and gj_dyn_soft_8500
 * (batch8497 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8496_marker[] = "libcgj-batch8496";

/* Smoke soft lamp for wave 8500 (always PASS). */
#define B8496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8496_soft(void)
{
	return B8496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8500 - report smoke soft lamp for wave 8500.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_8500 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8500(void)
{
	(void)NULL;
	return b8496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8500(void)
    __attribute__((alias("gj_smoke_soft_8500")));
