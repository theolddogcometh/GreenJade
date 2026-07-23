/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9497: dyn smoke soft gate (wave 9500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_9500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 9500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_9500  (alias)
 *   __libcgj_batch9497_marker = "libcgj-batch9497"
 *
 * Milestone 9500 exclusive continuum CREATE-ONLY (9491-9500). Unique
 * gj_dyn_soft_9500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9400 (batch9397), gj_dyn_soft_9300 (batch9297),
 * gj_dyn_soft_9200 (batch9197), and gj_smoke_soft_9500
 * (batch9496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9497_marker[] = "libcgj-batch9497";

/* Dyn soft lamp for wave 9500 (always soft-ready). */
#define B9497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9497_soft(void)
{
	return B9497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_9500 - report dyn soft lamp for wave 9500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_9500(void)
{
	(void)NULL;
	return b9497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_9500(void)
    __attribute__((alias("gj_dyn_soft_9500")));
