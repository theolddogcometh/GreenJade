/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13297: dyn smoke soft gate (wave 13300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13300(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13300  (alias)
 *   __libcgj_batch13297_marker = "libcgj-batch13297"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_dyn_soft_13300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13200 (batch13197), gj_dyn_soft_13100 (batch13097),
 * gj_dyn_soft_13000 (batch12997), and gj_smoke_soft_13300
 * (batch13296 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13297_marker[] = "libcgj-batch13297";

/* Dyn soft lamp for wave 13300 (always soft-ready). */
#define B13297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13297_soft(void)
{
	return B13297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13300 - report dyn soft lamp for wave 13300.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13296) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13300(void)
{
	(void)NULL;
	return b13297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13300(void)
    __attribute__((alias("gj_dyn_soft_13300")));
