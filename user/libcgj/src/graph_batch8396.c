/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8396: static smoke soft-green gate (wave 8400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8400(void);
 *     - Returns 1 (smoke soft-green PASS lamp for the milestone 8400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8400  (alias)
 *   __libcgj_batch8396_marker = "libcgj-batch8396"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_smoke_soft_8400 surface only; no multi-def. Distinct from
 * gj_smoke_green_8200 (batch8193), gj_smoke_green_8100 (batch8093),
 * gj_smoke_green_8000 (batch7993), and gj_dyn_soft_8400
 * (batch8397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8396_marker[] = "libcgj-batch8396";

/* Smoke soft-green lamp for wave 8400 (always PASS). */
#define B8396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8396_soft(void)
{
	return B8396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8400 - report smoke soft-green lamp for wave 8400.
 *
 * Always returns 1 (soft-green PASS). Pair with gj_dyn_soft_8400 for
 * the dyn soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8400(void)
{
	(void)NULL;
	return b8396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8400(void)
    __attribute__((alias("gj_smoke_soft_8400")));
