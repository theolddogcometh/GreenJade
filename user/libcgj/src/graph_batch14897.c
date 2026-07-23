/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14897: dyn soft lamp (wave 14900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_14900(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 14900
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_14900  (alias)
 *   __libcgj_batch14897_marker = "libcgj-batch14897"
 *
 * Milestone 14900 exclusive continuum CREATE-ONLY (14891-14900). Unique surface
 * only; no multi-def. Distinct from gj_*_14800 / gj_*_14700 / gj_*_14600 milestone
 * surfaces and sibling 14900 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14897_marker[] = "libcgj-batch14897";

/* Dyn soft ready lamp for wave 14900. */
#define B14897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14897_soft(void)
{
	return B14897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_14900 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_dyn_soft_14900(void)
{
	(void)NULL;
	return b14897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_14900(void)
    __attribute__((alias("gj_dyn_soft_14900")));
