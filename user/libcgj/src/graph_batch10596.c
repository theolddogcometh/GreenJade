/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10596: static smoke soft gate (wave 10600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_10600(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 10600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_10600  (alias)
 *   __libcgj_batch10596_marker = "libcgj-batch10596"
 *
 * Milestone 10600 exclusive continuum CREATE-ONLY (10591-10600). Unique
 * gj_smoke_soft_10600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_10500 (batch10496), gj_smoke_soft_10400 (batch10396),
 * gj_smoke_soft_10300 (batch10296), and gj_dyn_soft_10600
 * (batch10597 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10596_marker[] = "libcgj-batch10596";

/* Smoke soft lamp for wave 10600 (always PASS). */
#define B10596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10596_soft(void)
{
	return B10596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_10600 - report smoke soft lamp for wave 10600.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_10600 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_10600(void)
{
	(void)NULL;
	return b10596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_10600(void)
    __attribute__((alias("gj_smoke_soft_10600")));
