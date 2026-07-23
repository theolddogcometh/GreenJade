/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14097: dyn smoke soft gate (wave 14100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_14100(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 14100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_14100  (alias)
 *   __libcgj_batch14097_marker = "libcgj-batch14097"
 *
 * Milestone 14100 exclusive continuum CREATE-ONLY (14091-14100). Unique
 * gj_dyn_soft_14100 surface only; no multi-def. Distinct from
 * gj_dyn_soft_14000 (batch13997), gj_dyn_soft_13000 (batch12997),
 * gj_dyn_soft_12900 (batch12897), and gj_smoke_soft_14100
 * (batch14096 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14097_marker[] = "libcgj-batch14097";

/* Dyn soft lamp for wave 14100 (always soft-ready). */
#define B14097_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14097_soft(void)
{
	return B14097_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_14100 - report dyn soft lamp for wave 14100.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch14096) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_14100(void)
{
	(void)NULL;
	return b14097_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_14100(void)
    __attribute__((alias("gj_dyn_soft_14100")));
