/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11897: dyn smoke soft gate (wave 11900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_11900(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 11900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_11900  (alias)
 *   __libcgj_batch11897_marker = "libcgj-batch11897"
 *
 * Milestone 11900 exclusive continuum CREATE-ONLY (11891-11900). Unique
 * gj_dyn_soft_11900 surface only; no multi-def. Distinct from
 * gj_dyn_soft_11800 (batch11797), gj_dyn_soft_11700 (batch11697),
 * gj_dyn_soft_11600 (batch11597), and gj_smoke_soft_11900
 * (batch11896 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11897_marker[] = "libcgj-batch11897";

/* Dyn soft lamp for wave 11900 (always soft-ready). */
#define B11897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11897_soft(void)
{
	return B11897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_11900 - report dyn soft lamp for wave 11900.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch11896) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_11900(void)
{
	(void)NULL;
	return b11897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_11900(void)
    __attribute__((alias("gj_dyn_soft_11900")));
