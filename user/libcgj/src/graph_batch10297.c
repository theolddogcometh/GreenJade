/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10297: dyn smoke soft gate (wave 10300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_10300(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 10300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_10300  (alias)
 *   __libcgj_batch10297_marker = "libcgj-batch10297"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_dyn_soft_10300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_10200 (batch10197), gj_dyn_soft_10100 (batch10097),
 * gj_dyn_soft_10000 (batch9997), and gj_smoke_soft_10300
 * (batch10296 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10297_marker[] = "libcgj-batch10297";

/* Dyn soft lamp for wave 10300 (always soft-ready). */
#define B10297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10297_soft(void)
{
	return B10297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_10300 - report dyn soft lamp for wave 10300.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch10296) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_10300(void)
{
	(void)NULL;
	return b10297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_10300(void)
    __attribute__((alias("gj_dyn_soft_10300")));
