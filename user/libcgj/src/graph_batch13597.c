/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13597: dyn smoke soft gate (wave 13600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13600(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13600  (alias)
 *   __libcgj_batch13597_marker = "libcgj-batch13597"
 *
 * Milestone 13600 exclusive continuum CREATE-ONLY (13591-13600). Unique
 * gj_dyn_soft_13600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13500 (batch13497), gj_dyn_soft_13400 (batch13397),
 * gj_dyn_soft_13300 (batch13297), and gj_smoke_soft_13600
 * (batch13596 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13597_marker[] = "libcgj-batch13597";

/* Dyn soft lamp for wave 13600 (always soft-ready). */
#define B13597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13597_soft(void)
{
	return B13597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13600 - report dyn soft lamp for wave 13600.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13596) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13600(void)
{
	(void)NULL;
	return b13597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13600(void)
    __attribute__((alias("gj_dyn_soft_13600")));
