/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13996: static smoke soft gate (wave 14000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_14000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 14000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_14000  (alias)
 *   __libcgj_batch13996_marker = "libcgj-batch13996"
 *
 * Milestone 14000 exclusive continuum CREATE-ONLY (13991-14000). Unique
 * gj_smoke_soft_14000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13000 (batch12996), gj_smoke_soft_12900 (batch12896),
 * gj_smoke_soft_12800 (batch12796), and gj_dyn_soft_14000
 * (batch13997 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13996_marker[] = "libcgj-batch13996";

/* Smoke soft lamp for wave 14000 (always PASS). */
#define B13996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13996_soft(void)
{
	return B13996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_14000 - report smoke soft lamp for wave 14000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_14000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14000(void)
{
	(void)NULL;
	return b13996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_14000(void)
    __attribute__((alias("gj_smoke_soft_14000")));
