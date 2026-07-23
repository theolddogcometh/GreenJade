/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8596: static smoke soft gate (wave 8600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 8600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8600  (alias)
 *   __libcgj_batch8596_marker = "libcgj-batch8596"
 *
 * Milestone 8600 exclusive continuum CREATE-ONLY (8591-8600). Unique
 * gj_smoke_soft_8600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_8500 (batch8496), gj_smoke_soft_8400 (batch8396),
 * gj_smoke_soft_8300 (batch8296), and gj_dyn_soft_8600
 * (batch8597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8596_marker[] = "libcgj-batch8596";

/* Smoke soft lamp for wave 8600 (always PASS). */
#define B8596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8596_soft(void)
{
	return B8596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8600 - report smoke soft lamp for wave 8600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_8600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8600(void)
{
	(void)NULL;
	return b8596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8600(void)
    __attribute__((alias("gj_smoke_soft_8600")));
