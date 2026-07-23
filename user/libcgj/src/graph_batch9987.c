/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9987: dyn smoke soft gate (wave 10000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_10000(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 10000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_10000  (alias)
 *   __libcgj_batch9987_marker = "libcgj-batch9987"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990). Unique
 * gj_dyn_soft_10000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_9900 (batch9897), gj_dyn_soft_9800 (batch9797),
 * gj_dyn_soft_9700 (batch9697), and gj_smoke_soft_10000
 * (batch9986 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9987_marker[] = "libcgj-batch9987";

/* Dyn soft lamp for wave 10000 (always soft-ready). */
#define B9987_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9987_soft(void)
{
	return B9987_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_10000 - report dyn soft lamp for wave 10000.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch9986) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_10000(void)
{
	(void)NULL;
	return b9987_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_10000(void)
    __attribute__((alias("gj_dyn_soft_10000")));
