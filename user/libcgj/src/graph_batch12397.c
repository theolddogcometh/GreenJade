/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12397: dyn smoke soft gate (wave 12400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12400(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12400  (alias)
 *   __libcgj_batch12397_marker = "libcgj-batch12397"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_dyn_soft_12400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12300 (batch12297), gj_dyn_soft_12200 (batch12197),
 * gj_dyn_soft_12100 (batch12097), and gj_smoke_soft_12400
 * (batch12396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12397_marker[] = "libcgj-batch12397";

/* Dyn soft lamp for wave 12400 (always soft-ready). */
#define B12397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12397_soft(void)
{
	return B12397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12400 - report dyn soft lamp for wave 12400.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12400(void)
{
	(void)NULL;
	return b12397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12400(void)
    __attribute__((alias("gj_dyn_soft_12400")));
