/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9197: dyn smoke soft gate (wave 9200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9200(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9200  (alias)
 *   __libcgj_batch9197_marker = "libcgj-batch9197"
 *
 * Milestone 9200 exclusive continuum CREATE-ONLY (9191-9200). Unique
 * gj_dyn_soft_9200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9100 (batch9097), gj_dyn_soft_9000 (batch8997),
 * gj_dyn_soft_8900 (batch8897), and gj_smoke_soft_9200
 * (batch9196 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9197_marker[] = "libcgj-batch9197";

/* Dyn soft lamp for wave 9200 (always soft-ready). */
#define B9197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9197_soft(void)
{
	return B9197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9200 - report dyn soft lamp for wave 9200.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9196) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9200(void)
{
	(void)NULL;
	return b9197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9200(void)
    __attribute__((alias("gj_dyn_soft_9200")));
