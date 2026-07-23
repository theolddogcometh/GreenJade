/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9397: dyn smoke soft gate (wave 9400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9400(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9400  (alias)
 *   __libcgj_batch9397_marker = "libcgj-batch9397"
 *
 * Milestone 9400 exclusive continuum CREATE-ONLY (9391-9400). Unique
 * gj_dyn_soft_9400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9300 (batch9297), gj_dyn_soft_9200 (batch9197),
 * gj_dyn_soft_9100 (batch9097), and gj_smoke_soft_9400
 * (batch9396 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9397_marker[] = "libcgj-batch9397";

/* Dyn soft lamp for wave 9400 (always soft-ready). */
#define B9397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9397_soft(void)
{
	return B9397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9400 - report dyn soft lamp for wave 9400.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9396) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9400(void)
{
	(void)NULL;
	return b9397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9400(void)
    __attribute__((alias("gj_dyn_soft_9400")));
