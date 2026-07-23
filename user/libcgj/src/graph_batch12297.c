/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12297: dyn smoke soft gate (wave 12300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_12300(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 12300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_12300  (alias)
 *   __libcgj_batch12297_marker = "libcgj-batch12297"
 *
 * Milestone 12300 exclusive continuum CREATE-ONLY (12291-12300). Unique
 * gj_dyn_soft_12300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_12200 (batch12197), gj_dyn_soft_12100 (batch12097),
 * gj_dyn_soft_12000 (batch11997), and gj_smoke_soft_12300
 * (batch12296 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12297_marker[] = "libcgj-batch12297";

/* Dyn soft lamp for wave 12300 (always soft-ready). */
#define B12297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12297_soft(void)
{
	return B12297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_12300 - report dyn soft lamp for wave 12300.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch12296) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_12300(void)
{
	(void)NULL;
	return b12297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_12300(void)
    __attribute__((alias("gj_dyn_soft_12300")));
