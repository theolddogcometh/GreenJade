/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13497: dyn smoke soft gate (wave 13500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13500  (alias)
 *   __libcgj_batch13497_marker = "libcgj-batch13497"
 *
 * Milestone 13500 exclusive continuum CREATE-ONLY (13491-13500). Unique
 * gj_dyn_soft_13500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13400 (batch13397), gj_dyn_soft_13300 (batch13297),
 * gj_dyn_soft_13200 (batch13197), and gj_smoke_soft_13500
 * (batch13496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13497_marker[] = "libcgj-batch13497";

/* Dyn soft lamp for wave 13500 (always soft-ready). */
#define B13497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13497_soft(void)
{
	return B13497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13500 - report dyn soft lamp for wave 13500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13500(void)
{
	(void)NULL;
	return b13497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13500(void)
    __attribute__((alias("gj_dyn_soft_13500")));
