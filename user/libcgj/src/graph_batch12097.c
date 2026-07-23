/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12097: dyn smoke soft gate (wave 12100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12100(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12100  (alias)
 *   __libcgj_batch12097_marker = "libcgj-batch12097"
 *
 * Milestone 12100 exclusive continuum CREATE-ONLY (12091-12100). Unique
 * gj_dyn_soft_12100 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12000 (batch11997), gj_dyn_soft_11900 (batch11897),
 * gj_dyn_soft_11800 (batch11797), and gj_smoke_soft_12100
 * (batch12096 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12097_marker[] = "libcgj-batch12097";

/* Dyn soft lamp for wave 12100 (always soft-ready). */
#define B12097_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12097_soft(void)
{
	return B12097_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12100 - report dyn soft lamp for wave 12100.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12096) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12100(void)
{
	(void)NULL;
	return b12097_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12100(void)
    __attribute__((alias("gj_dyn_soft_12100")));
