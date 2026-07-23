/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12897: dyn smoke soft gate (wave 12900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12900(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12900  (alias)
 *   __libcgj_batch12897_marker = "libcgj-batch12897"
 *
 * Milestone 12900 exclusive continuum CREATE-ONLY (12891-12900). Unique
 * gj_dyn_soft_12900 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12800 (batch12797), gj_dyn_soft_12700 (batch12697),
 * gj_dyn_soft_12600 (batch12597), and gj_smoke_soft_12900
 * (batch12896 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12897_marker[] = "libcgj-batch12897";

/* Dyn soft lamp for wave 12900 (always soft-ready). */
#define B12897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12897_soft(void)
{
	return B12897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12900 - report dyn soft lamp for wave 12900.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12896) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12900(void)
{
	(void)NULL;
	return b12897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12900(void)
    __attribute__((alias("gj_dyn_soft_12900")));
