/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8497: dyn smoke soft gate (wave 8500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_8500(void);
 *     - Returns 1 (dyn-smoke soft lamp for the milestone 8500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_soft_8500  (alias)
 *   __libcgj_batch8497_marker = "libcgj-batch8497"
 *
 * Milestone 8500 exclusive continuum CREATE-ONLY (8491-8500). Unique
 * gj_dyn_soft_8500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_8300 (batch8297), gj_dyn_green_8200 (batch8194),
 * gj_dyn_green_8000 (batch7994), and gj_smoke_soft_8500
 * (batch8496 soft PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8497_marker[] = "libcgj-batch8497";

/* Dyn soft lamp for wave 8500 (always soft-ready). */
#define B8497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8497_soft(void)
{
	return B8497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_8500 - report dyn soft lamp for wave 8500.
 *
 * Always returns 1 (soft-ready). Distinct from static smoke soft
 * (batch8496) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_soft_8500(void)
{
	(void)NULL;
	return b8497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_soft_8500(void)
    __attribute__((alias("gj_dyn_soft_8500")));
