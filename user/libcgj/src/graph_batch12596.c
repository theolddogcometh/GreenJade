/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12596: static smoke soft gate (wave 12600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12600  (alias)
 *   __libcgj_batch12596_marker = "libcgj-batch12596"
 *
 * Milestone 12600 exclusive continuum CREATE-ONLY (12591-12600). Unique
 * gj_smoke_soft_12600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12500 (batch12496), gj_smoke_soft_12400 (batch12396),
 * gj_smoke_soft_12300 (batch12296), and gj_dyn_soft_12600
 * (batch12597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12596_marker[] = "libcgj-batch12596";

/* Smoke soft lamp for wave 12600 (always PASS). */
#define B12596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12596_soft(void)
{
	return B12596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12600 - report smoke soft lamp for wave 12600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12600(void)
{
	(void)NULL;
	return b12596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12600(void)
    __attribute__((alias("gj_smoke_soft_12600")));
