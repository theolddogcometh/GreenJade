/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11997: dyn smoke soft gate (wave 12000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12000(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12000  (alias)
 *   __libcgj_batch11997_marker = "libcgj-batch11997"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_dyn_soft_12000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_11900 (batch11897), gj_dyn_soft_11800 (batch11797),
 * gj_dyn_soft_11700 (batch11697), and gj_smoke_soft_12000
 * (batch11996 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11997_marker[] = "libcgj-batch11997";

/* Dyn soft lamp for wave 12000 (always soft-ready). */
#define B11997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11997_soft(void)
{
	return B11997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12000 - report dyn soft lamp for wave 12000.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch11996) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12000(void)
{
	(void)NULL;
	return b11997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12000(void)
    __attribute__((alias("gj_dyn_soft_12000")));
