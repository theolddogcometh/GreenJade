/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14496: smoke soft PASS lamp (wave 14500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_14500(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 14500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_14500  (alias)
 *   __libcgj_batch14496_marker = "libcgj-batch14496"
 *
 * Milestone 14500 exclusive continuum CREATE-ONLY (14491-14500). Unique gj_smoke_soft_14500
 * surface only; no multi-def. Distinct from gj_*_14400 / gj_*_14300 / gj_*_14200 milestone
 * surfaces and sibling 14500 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14496_marker[] = "libcgj-batch14496";

/* Smoke soft PASS lamp for wave 14500. */
#define B14496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14496_soft(void)
{
	return B14496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_14500 - smoke soft lamp for wave 14500
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14500(void)
{
	(void)NULL;
	return b14496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_14500(void)
    __attribute__((alias("gj_smoke_soft_14500")));
