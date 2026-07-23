/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14697: dyn soft lamp (wave 14700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_14700(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 14700
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_14700  (alias)
 *   __libcgj_batch14697_marker = "libcgj-batch14697"
 *
 * Milestone 14700 exclusive continuum CREATE-ONLY (14691-14700). Unique surface
 * only; no multi-def. Distinct from gj_*_14600 / gj_*_14500 / gj_*_14400 milestone
 * surfaces and sibling 14700 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14697_marker[] = "libcgj-batch14697";

/* Dyn soft ready lamp for wave 14700. */
#define B14697_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14697_soft(void)
{
	return B14697_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_14700 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_dyn_soft_14700(void)
{
	(void)NULL;
	return b14697_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_14700(void)
    __attribute__((alias("gj_dyn_soft_14700")));
