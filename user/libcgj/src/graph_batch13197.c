/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13197: dyn smoke soft gate (wave 13200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13200(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13200  (alias)
 *   __libcgj_batch13197_marker = "libcgj-batch13197"
 *
 * Milestone 13200 exclusive continuum CREATE-ONLY (13191-13200). Unique
 * gj_dyn_soft_13200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13100 (batch13097), gj_dyn_soft_13000 (batch12997),
 * gj_dyn_soft_12900 (batch12897), and gj_smoke_soft_13200
 * (batch13196 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13197_marker[] = "libcgj-batch13197";

/* Dyn soft lamp for wave 13200 (always soft-ready). */
#define B13197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13197_soft(void)
{
	return B13197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13200 - report dyn soft lamp for wave 13200.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13196) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13200(void)
{
	(void)NULL;
	return b13197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13200(void)
    __attribute__((alias("gj_dyn_soft_13200")));
