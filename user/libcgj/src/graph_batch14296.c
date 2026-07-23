/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14296: static smoke soft gate (wave 14300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_14300(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 14300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_14300  (alias)
 *   __libcgj_batch14296_marker = "libcgj-batch14296"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_smoke_soft_14300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_14200 (batch14196), gj_smoke_soft_14100 (batch14096),
 * gj_smoke_soft_14000 (batch13996), and gj_dyn_soft_14300
 * (batch14297 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14296_marker[] = "libcgj-batch14296";

/* Smoke soft lamp for wave 14300 (always PASS). */
#define B14296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14296_soft(void)
{
	return B14296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_14300 - report smoke soft lamp for wave 14300.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_14300 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14300(void)
{
	(void)NULL;
	return b14296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_14300(void)
    __attribute__((alias("gj_smoke_soft_14300")));
