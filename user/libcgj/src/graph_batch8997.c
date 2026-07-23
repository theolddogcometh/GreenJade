/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8997: dyn smoke soft gate (wave 9000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9000(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9000  (alias)
 *   __libcgj_batch8997_marker = "libcgj-batch8997"
 *
 * Milestone 9000 exclusive continuum CREATE-ONLY (8991-9000). Unique
 * gj_dyn_soft_9000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_8900 (batch8897), gj_dyn_soft_8800 (batch8797),
 * gj_dyn_soft_8700 (batch8697), and gj_smoke_soft_9000
 * (batch8996 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8997_marker[] = "libcgj-batch8997";

/* Dyn soft lamp for wave 9000 (always soft-ready). */
#define B8997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8997_soft(void)
{
	return B8997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9000 - report dyn soft lamp for wave 9000.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch8996) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9000(void)
{
	(void)NULL;
	return b8997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9000(void)
    __attribute__((alias("gj_dyn_soft_9000")));
