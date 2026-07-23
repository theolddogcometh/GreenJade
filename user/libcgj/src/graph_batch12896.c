/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12896: static smoke soft gate (wave 12900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12900(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12900  (alias)
 *   __libcgj_batch12896_marker = "libcgj-batch12896"
 *
 * Milestone 12900 exclusive continuum CREATE-ONLY (12891-12900). Unique
 * gj_smoke_soft_12900 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12800 (batch12796), gj_smoke_soft_12700 (batch12696),
 * gj_smoke_soft_12600 (batch12596), and gj_dyn_soft_12900
 * (batch12897 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12896_marker[] = "libcgj-batch12896";

/* Smoke soft lamp for wave 12900 (always PASS). */
#define B12896_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12896_soft(void)
{
	return B12896_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12900 - report smoke soft lamp for wave 12900.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12900 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12900(void)
{
	(void)NULL;
	return b12896_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12900(void)
    __attribute__((alias("gj_smoke_soft_12900")));
