/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13697: dyn smoke soft gate (wave 13700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13700(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13700  (alias)
 *   __libcgj_batch13697_marker = "libcgj-batch13697"
 *
 * Milestone 13700 exclusive continuum CREATE-ONLY (13691-13700). Unique
 * gj_dyn_soft_13700 surface only; no multi-def. Distinct from
 * gj_dyn_soft_13600 (batch13597), gj_dyn_soft_13500 (batch13497),
 * gj_dyn_soft_13400 (batch13397), and gj_smoke_soft_13700
 * (batch13696 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13697_marker[] = "libcgj-batch13697";

/* Dyn soft lamp for wave 13700 (always soft-ready). */
#define B13697_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13697_soft(void)
{
	return B13697_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13700 - report dyn soft lamp for wave 13700.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch13696) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13700(void)
{
	(void)NULL;
	return b13697_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13700(void)
    __attribute__((alias("gj_dyn_soft_13700")));
