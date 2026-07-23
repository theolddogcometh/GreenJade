/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11996: static smoke soft gate (wave 12000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12000  (alias)
 *   __libcgj_batch11996_marker = "libcgj-batch11996"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_smoke_soft_12000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_11900 (batch11896), gj_smoke_soft_11800 (batch11796),
 * gj_smoke_soft_11700 (batch11696), and gj_dyn_soft_12000
 * (batch11997 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11996_marker[] = "libcgj-batch11996";

/* Smoke soft lamp for wave 12000 (always PASS). */
#define B11996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11996_soft(void)
{
	return B11996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12000 - report smoke soft lamp for wave 12000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12000(void)
{
	(void)NULL;
	return b11996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12000(void)
    __attribute__((alias("gj_smoke_soft_12000")));
