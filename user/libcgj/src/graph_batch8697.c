/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8697: dyn smoke soft gate (wave 8700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_8700(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 8700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_8700  (alias)
 *   __libcgj_batch8697_marker = "libcgj-batch8697"
 *
 * Milestone 8700 exclusive continuum CREATE-ONLY (8691-8700). Unique
 * gj_dyn_soft_8700 surface only; no multi-def. Distinct from
 * gj_dyn_soft_8600 (batch8597), gj_dyn_soft_8500 (batch8497),
 * gj_dyn_soft_8400 (batch8397), and gj_smoke_soft_8700
 * (batch8696 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8697_marker[] = "libcgj-batch8697";

/* Dyn soft lamp for wave 8700 (always soft-ready). */
#define B8697_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8697_soft(void)
{
	return B8697_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_8700 - report dyn soft lamp for wave 8700.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch8696) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_8700(void)
{
	(void)NULL;
	return b8697_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_8700(void)
    __attribute__((alias("gj_dyn_soft_8700")));
