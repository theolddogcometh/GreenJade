/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12497: dyn smoke soft gate (wave 12500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12500  (alias)
 *   __libcgj_batch12497_marker = "libcgj-batch12497"
 *
 * Milestone 12500 exclusive continuum CREATE-ONLY (12491-12500). Unique
 * gj_dyn_soft_12500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12400 (batch12397), gj_dyn_soft_12300 (batch12297),
 * gj_dyn_soft_12200 (batch12197), and gj_smoke_soft_12500
 * (batch12496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12497_marker[] = "libcgj-batch12497";

/* Dyn soft lamp for wave 12500 (always soft-ready). */
#define B12497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12497_soft(void)
{
	return B12497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12500 - report dyn soft lamp for wave 12500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12500(void)
{
	(void)NULL;
	return b12497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12500(void)
    __attribute__((alias("gj_dyn_soft_12500")));
