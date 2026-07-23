/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13397: dyn smoke soft gate (wave 13400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13400(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13400  (alias)
 *   __libcgj_batch13397_marker = "libcgj-batch13397"
 *
 * Milestone 13400 exclusive continuum CREATE-ONLY (13391-13400). Unique
 * gj_dyn_soft_13400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13300 (batch13297), gj_dyn_soft_13200 (batch13197),
 * gj_dyn_soft_13100 (batch13097), and gj_smoke_soft_13400
 * (batch13396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13397_marker[] = "libcgj-batch13397";

/* Dyn soft lamp for wave 13400 (always soft-ready). */
#define B13397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13397_soft(void)
{
	return B13397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13400 - report dyn soft lamp for wave 13400.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13400(void)
{
	(void)NULL;
	return b13397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13400(void)
    __attribute__((alias("gj_dyn_soft_13400")));
