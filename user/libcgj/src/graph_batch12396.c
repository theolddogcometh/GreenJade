/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12396: static smoke soft gate (wave 12400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12400  (alias)
 *   __libcgj_batch12396_marker = "libcgj-batch12396"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_smoke_soft_12400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12300 (batch12296), gj_smoke_soft_12200 (batch12196),
 * gj_smoke_soft_12100 (batch12096), and gj_dyn_soft_12400
 * (batch12397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12396_marker[] = "libcgj-batch12396";

/* Smoke soft lamp for wave 12400 (always PASS). */
#define B12396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12396_soft(void)
{
	return B12396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12400 - report smoke soft lamp for wave 12400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12400(void)
{
	(void)NULL;
	return b12396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12400(void)
    __attribute__((alias("gj_smoke_soft_12400")));
