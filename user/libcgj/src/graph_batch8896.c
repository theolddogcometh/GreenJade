/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8896: static smoke soft gate (wave 8900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_8900(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 8900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_8900  (alias)
 *   __libcgj_batch8896_marker = "libcgj-batch8896"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_smoke_soft_8900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_8800 (batch8796), gj_smoke_soft_8700 (batch8696),
 * gj_smoke_soft_8600 (batch8596), and gj_dyn_soft_8900
 * (batch8897 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8896_marker[] = "libcgj-batch8896";

/* Smoke soft lamp for wave 8900 (always PASS). */
#define B8896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8896_soft(void)
{
	return B8896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_8900 - report smoke soft lamp for wave 8900.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_8900 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_8900(void)
{
	(void)NULL;
	return b8896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_8900(void)
    __attribute__((alias("gj_smoke_soft_8900")));
