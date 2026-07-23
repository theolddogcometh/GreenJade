/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12597: dyn smoke soft gate (wave 12600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12600(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12600  (alias)
 *   __libcgj_batch12597_marker = "libcgj-batch12597"
 *
 * Milestone 12600 exclusive continuum CREATE-ONLY (12591-12600). Unique
 * gj_dyn_soft_12600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12500 (batch12497), gj_dyn_soft_12400 (batch12397),
 * gj_dyn_soft_12300 (batch12297), and gj_smoke_soft_12600
 * (batch12596 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12597_marker[] = "libcgj-batch12597";

/* Dyn soft lamp for wave 12600 (always soft-ready). */
#define B12597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12597_soft(void)
{
	return B12597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12600 - report dyn soft lamp for wave 12600.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12596) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12600(void)
{
	(void)NULL;
	return b12597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12600(void)
    __attribute__((alias("gj_dyn_soft_12600")));
