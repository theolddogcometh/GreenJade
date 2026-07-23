/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10897: dyn smoke soft gate (wave 10900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_10900(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 10900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_10900  (alias)
 *   __libcgj_batch10897_marker = "libcgj-batch10897"
 *
 * Milestone 10900 exclusive continuum CREATE-ONLY (10891-10900). Unique
 * gj_dyn_soft_10900 surface only; no multi-def. Distinct from
 * gj_dyn_soft_10800 (batch10797), gj_dyn_soft_10700 (batch10697),
 * gj_dyn_soft_10600 (batch10597), and gj_smoke_soft_10900
 * (batch10896 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10897_marker[] = "libcgj-batch10897";

/* Dyn soft lamp for wave 10900 (always soft-ready). */
#define B10897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10897_soft(void)
{
	return B10897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_10900 - report dyn soft lamp for wave 10900.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch10896) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_10900(void)
{
	(void)NULL;
	return b10897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_10900(void)
    __attribute__((alias("gj_dyn_soft_10900")));
