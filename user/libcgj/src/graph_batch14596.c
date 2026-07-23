/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14596: smoke soft lamp (wave 14600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_14600(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 14600
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_14600  (alias)
 *   __libcgj_batch14596_marker = "libcgj-batch14596"
 *
 * Milestone 14600 exclusive continuum CREATE-ONLY (14591-14600). Unique surface
 * only; no multi-def. Distinct from gj_*_14500 / gj_*_14400 / gj_*_14300 milestone
 * surfaces and sibling 14600 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14596_marker[] = "libcgj-batch14596";

/* Smoke soft ready lamp for wave 14600. */
#define B14596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14596_soft(void)
{
	return B14596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_14600 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14600(void)
{
	(void)NULL;
	return b14596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_14600(void)
    __attribute__((alias("gj_smoke_soft_14600")));
