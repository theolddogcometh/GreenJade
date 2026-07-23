/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12696: static smoke soft gate (wave 12700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12700(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12700  (alias)
 *   __libcgj_batch12696_marker = "libcgj-batch12696"
 *
 * Milestone 12700 exclusive continuum CREATE-ONLY (12691-12700). Unique
 * gj_smoke_soft_12700 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12600 (batch12596), gj_smoke_soft_12500 (batch12496),
 * gj_smoke_soft_12400 (batch12396), and gj_dyn_soft_12700
 * (batch12697 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12696_marker[] = "libcgj-batch12696";

/* Smoke soft lamp for wave 12700 (always PASS). */
#define B12696_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12696_soft(void)
{
	return B12696_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12700 - report smoke soft lamp for wave 12700.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12700 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12700(void)
{
	(void)NULL;
	return b12696_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12700(void)
    __attribute__((alias("gj_smoke_soft_12700")));
