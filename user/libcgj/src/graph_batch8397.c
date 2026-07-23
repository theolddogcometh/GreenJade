/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8397: dyn smoke soft-green gate (wave 8400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_8400(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 8400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_8400  (alias)
 *   __libcgj_batch8397_marker = "libcgj-batch8397"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_dyn_soft_8400 surface only; no multi-def. Distinct from
 * gj_dyn_green_8200 (batch8194), gj_dyn_green_8100 (batch8094),
 * gj_dyn_green_8000 (batch7994), and gj_smoke_soft_8400
 * (batch8396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8397_marker[] = "libcgj-batch8397";

/* Dyn soft-green lamp for wave 8400 (always green). */
#define B8397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8397_soft(void)
{
	return B8397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_8400 - report dyn soft-green lamp for wave 8400.
 *
 * Always returns 1 (soft-green). Distinct from static smoke soft
 * (batch8396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_8400(void)
{
	(void)NULL;
	return b8397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_8400(void)
    __attribute__((alias("gj_dyn_soft_8400")));
