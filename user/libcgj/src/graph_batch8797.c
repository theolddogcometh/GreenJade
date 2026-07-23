/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8797: dyn smoke soft gate (wave 8800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_8800(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 8800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_8800  (alias)
 *   __libcgj_batch8797_marker = "libcgj-batch8797"
 *
 * Milestone 8800 exclusive continuum CREATE-ONLY (8791-8800). Unique
 * gj_dyn_soft_8800 surface only; no multi-def. Distinct from
 * gj_dyn_soft_8700 (batch8697), gj_dyn_soft_8600 (batch8597),
 * gj_dyn_soft_8500 (batch8497), and gj_smoke_soft_8800
 * (batch8796 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8797_marker[] = "libcgj-batch8797";

/* Dyn soft lamp for wave 8800 (always soft-ready). */
#define B8797_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8797_soft(void)
{
	return B8797_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_8800 - report dyn soft lamp for wave 8800.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch8796) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_8800(void)
{
	(void)NULL;
	return b8797_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_8800(void)
    __attribute__((alias("gj_dyn_soft_8800")));
