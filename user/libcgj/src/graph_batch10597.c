/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10597: dyn smoke soft gate (wave 10600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_10600(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 10600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_10600  (alias)
 *   __libcgj_batch10597_marker = "libcgj-batch10597"
 *
 * Milestone 10600 exclusive continuum CREATE-ONLY (10591-10600). Unique
 * gj_dyn_soft_10600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_10500 (batch10497), gj_dyn_soft_10400 (batch10397),
 * gj_dyn_soft_10300 (batch10297), and gj_smoke_soft_10600
 * (batch10596 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10597_marker[] = "libcgj-batch10597";

/* Dyn soft lamp for wave 10600 (always soft-ready). */
#define B10597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10597_soft(void)
{
	return B10597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_10600 - report dyn soft lamp for wave 10600.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch10596) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_10600(void)
{
	(void)NULL;
	return b10597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_10600(void)
    __attribute__((alias("gj_dyn_soft_10600")));
