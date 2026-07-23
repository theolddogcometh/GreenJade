/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12196: static smoke soft gate (wave 12200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12200(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12200
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12200  (alias)
 *   __libcgj_batch12196_marker = "libcgj-batch12196"
 *
 * Milestone 12200 exclusive continuum CREATE-ONLY (12191-12200). Unique
 * gj_smoke_soft_12200 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12100 (batch12096), gj_smoke_soft_12000 (batch11996),
 * gj_smoke_soft_11900 (batch11896), and gj_dyn_soft_12200
 * (batch12197 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12196_marker[] = "libcgj-batch12196";

/* Smoke soft lamp for wave 12200 (always PASS). */
#define B12196_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12196_soft(void)
{
	return B12196_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12200 - report smoke soft lamp for wave 12200.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12200 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12200(void)
{
	(void)NULL;
	return b12196_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12200(void)
    __attribute__((alias("gj_smoke_soft_12200")));
