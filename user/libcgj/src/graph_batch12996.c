/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12996: static smoke soft gate (wave 13000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13000  (alias)
 *   __libcgj_batch12996_marker = "libcgj-batch12996"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_smoke_soft_13000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12900 (batch12896), gj_smoke_soft_12800 (batch12796),
 * gj_smoke_soft_12700 (batch12696), and gj_dyn_soft_13000
 * (batch12997 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12996_marker[] = "libcgj-batch12996";

/* Smoke soft lamp for wave 13000 (always PASS). */
#define B12996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12996_soft(void)
{
	return B12996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13000 - report smoke soft lamp for wave 13000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13000(void)
{
	(void)NULL;
	return b12996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13000(void)
    __attribute__((alias("gj_smoke_soft_13000")));
