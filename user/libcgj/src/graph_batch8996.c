/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8996: static smoke soft gate (wave 9000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_9000(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 9000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_9000  (alias)
 *   __libcgj_batch8996_marker = "libcgj-batch8996"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_smoke_soft_9000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_8900 (batch8896), gj_smoke_soft_8800 (batch8796),
 * gj_smoke_soft_8700 (batch8696), and gj_dyn_soft_9000
 * (batch8997 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8996_marker[] = "libcgj-batch8996";

/* Smoke soft lamp for wave 9000 (always PASS). */
#define B8996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8996_soft(void)
{
	return B8996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_9000 - report smoke soft lamp for wave 9000.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_9000 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_9000(void)
{
	(void)NULL;
	return b8996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_9000(void)
    __attribute__((alias("gj_smoke_soft_9000")));
