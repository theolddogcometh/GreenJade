/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14197: dyn smoke soft gate (wave 14200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_14200(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 14200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_14200  (alias)
 *   __libcgj_batch14197_marker = "libcgj-batch14197"
 *
 * Milestone 14200 exclusive continuum CREATE-ONLY (14191-14200). Unique
 * gj_dyn_soft_14200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_14000 (batch13997), gj_dyn_soft_13000 (batch12997),
 * gj_dyn_soft_12900 (batch12897), and gj_smoke_soft_14200
 * (batch14196 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14197_marker[] = "libcgj-batch14197";

/* Dyn soft lamp for wave 14200 (always soft-ready). */
#define B14197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14197_soft(void)
{
	return B14197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_14200 - report dyn soft lamp for wave 14200.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch14196) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_14200(void)
{
	(void)NULL;
	return b14197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_14200(void)
    __attribute__((alias("gj_dyn_soft_14200")));
