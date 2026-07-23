/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9297: dyn smoke soft gate (wave 9300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9300(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9300  (alias)
 *   __libcgj_batch9297_marker = "libcgj-batch9297"
 *
 * Milestone 9300 exclusive continuum CREATE-ONLY (9291-9300). Unique
 * gj_dyn_soft_9300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9200 (batch9197), gj_dyn_soft_9100 (batch9097),
 * gj_dyn_soft_9000 (batch8997), and gj_smoke_soft_9300
 * (batch9296 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9297_marker[] = "libcgj-batch9297";

/* Dyn soft lamp for wave 9300 (always soft-ready). */
#define B9297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9297_soft(void)
{
	return B9297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9300 - report dyn soft lamp for wave 9300.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9296) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9300(void)
{
	(void)NULL;
	return b9297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9300(void)
    __attribute__((alias("gj_dyn_soft_9300")));
