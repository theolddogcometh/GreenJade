/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12496: static smoke soft gate (wave 12500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12500  (alias)
 *   __libcgj_batch12496_marker = "libcgj-batch12496"
 *
 * Milestone 12500 exclusive continuum CREATE-ONLY (12491-12500). Unique
 * gj_smoke_soft_12500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12400 (batch12396), gj_smoke_soft_12300 (batch12296),
 * gj_smoke_soft_12200 (batch12196), and gj_dyn_soft_12500
 * (batch12497 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12496_marker[] = "libcgj-batch12496";

/* Smoke soft lamp for wave 12500 (always PASS). */
#define B12496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12496_soft(void)
{
	return B12496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12500 - report smoke soft lamp for wave 12500.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12500 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12500(void)
{
	(void)NULL;
	return b12496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12500(void)
    __attribute__((alias("gj_smoke_soft_12500")));
