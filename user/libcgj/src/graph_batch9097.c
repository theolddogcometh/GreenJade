/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9097: dyn smoke soft gate (wave 9100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9100(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9100
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9100  (alias)
 *   __libcgj_batch9097_marker = "libcgj-batch9097"
 *
 * Milestone 9100 exclusive continuum CREATE-ONLY (9091-9100). Unique
 * gj_dyn_soft_9100 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9000 (batch8997), gj_dyn_soft_8900 (batch8897),
 * gj_dyn_soft_8800 (batch8797), and gj_smoke_soft_9100
 * (batch9096 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9097_marker[] = "libcgj-batch9097";

/* Dyn soft lamp for wave 9100 (always soft-ready). */
#define B9097_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9097_soft(void)
{
	return B9097_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9100 - report dyn soft lamp for wave 9100.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9096) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9100(void)
{
	(void)NULL;
	return b9097_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9100(void)
    __attribute__((alias("gj_dyn_soft_9100")));
