/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11797: dyn smoke soft gate (wave 11800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_11800(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 11800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_11800  (alias)
 *   __libcgj_batch11797_marker = "libcgj-batch11797"
 *
 * Milestone 11800 exclusive continuum CREATE-ONLY (11791-11800). Unique
 * gj_dyn_soft_11800 surface only; no multi-def. Distinct from
 * gj_dyn_soft_11700 (batch11697), gj_dyn_soft_11600 (batch11597),
 * gj_dyn_soft_11500 (batch11497), and gj_smoke_soft_11800
 * (batch11796 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11797_marker[] = "libcgj-batch11797";

/* Dyn soft lamp for wave 11800 (always soft-ready). */
#define B11797_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11797_soft(void)
{
	return B11797_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_11800 - report dyn soft lamp for wave 11800.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch11796) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_11800(void)
{
	(void)NULL;
	return b11797_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_11800(void)
    __attribute__((alias("gj_dyn_soft_11800")));
