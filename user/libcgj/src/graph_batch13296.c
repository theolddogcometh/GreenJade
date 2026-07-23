/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13296: static smoke soft gate (wave 13300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_13300(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 13300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_13300  (alias)
 *   __libcgj_batch13296_marker = "libcgj-batch13296"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_smoke_soft_13300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_13200 (batch13196), gj_smoke_soft_13100 (batch13096),
 * gj_smoke_soft_13000 (batch12996), and gj_dyn_soft_13300
 * (batch13297 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13296_marker[] = "libcgj-batch13296";

/* Smoke soft lamp for wave 13300 (always PASS). */
#define B13296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13296_soft(void)
{
	return B13296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_13300 - report smoke soft lamp for wave 13300.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_13300 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_13300(void)
{
	(void)NULL;
	return b13296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_13300(void)
    __attribute__((alias("gj_smoke_soft_13300")));
