/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8597: dyn smoke soft gate (wave 8600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_8600(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 8600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_8600  (alias)
 *   __libcgj_batch8597_marker = "libcgj-batch8597"
 *
 * Milestone 8600 exclusive continuum CREATE-ONLY (8591-8600). Unique
 * gj_dyn_soft_8600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_8500 (batch8497), gj_dyn_soft_8400 (batch8397),
 * gj_dyn_soft_8300 (batch8297), and gj_smoke_soft_8600
 * (batch8596 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8597_marker[] = "libcgj-batch8597";

/* Dyn soft lamp for wave 8600 (always soft-ready). */
#define B8597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8597_soft(void)
{
	return B8597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_8600 - report dyn soft lamp for wave 8600.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch8596) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_8600(void)
{
	(void)NULL;
	return b8597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_8600(void)
    __attribute__((alias("gj_dyn_soft_8600")));
