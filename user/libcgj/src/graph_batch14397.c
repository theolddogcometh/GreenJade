/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14397: dyn smoke soft gate (wave 14400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_14400(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 14400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_14400  (alias)
 *   __libcgj_batch14397_marker = "libcgj-batch14397"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_dyn_soft_14400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_14000 (batch13997), gj_dyn_soft_13000 (batch12997),
 * gj_dyn_soft_12900 (batch12897), and gj_smoke_soft_14400
 * (batch14396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14397_marker[] = "libcgj-batch14397";

/* Dyn soft lamp for wave 14400 (always soft-ready). */
#define B14397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14397_soft(void)
{
	return B14397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_14400 - report dyn soft lamp for wave 14400.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch14396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_14400(void)
{
	(void)NULL;
	return b14397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_14400(void)
    __attribute__((alias("gj_dyn_soft_14400")));
