/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14396: static smoke soft gate (wave 14400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_14400(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 14400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_14400  (alias)
 *   __libcgj_batch14396_marker = "libcgj-batch14396"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_smoke_soft_14400 surface only; no multi-def. Distinct from
 * gj_smoke_soft_14000 (batch13996), gj_smoke_soft_13000 (batch12996),
 * gj_smoke_soft_12900 (batch12896), and gj_dyn_soft_14400
 * (batch14397 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14396_marker[] = "libcgj-batch14396";

/* Smoke soft lamp for wave 14400 (always PASS). */
#define B14396_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14396_soft(void)
{
	return B14396_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_14400 - report smoke soft lamp for wave 14400.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_14400 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14400(void)
{
	(void)NULL;
	return b14396_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_14400(void)
    __attribute__((alias("gj_smoke_soft_14400")));
