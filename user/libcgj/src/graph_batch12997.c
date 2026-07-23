/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12997: dyn smoke soft gate (wave 13000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_13000(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 13000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_13000  (alias)
 *   __libcgj_batch12997_marker = "libcgj-batch12997"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_dyn_soft_13000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12900 (batch12897), gj_dyn_soft_12800 (batch12797),
 * gj_dyn_soft_12700 (batch12697), and gj_smoke_soft_13000
 * (batch12996 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12997_marker[] = "libcgj-batch12997";

/* Dyn soft lamp for wave 13000 (always soft-ready). */
#define B12997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12997_soft(void)
{
	return B12997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_13000 - report dyn soft lamp for wave 13000.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12996) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_13000(void)
{
	(void)NULL;
	return b12997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_13000(void)
    __attribute__((alias("gj_dyn_soft_13000")));
