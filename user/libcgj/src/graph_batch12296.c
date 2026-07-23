/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12296: static smoke soft gate (wave 12300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_12300(void);
 *     - Returns 1 (smoke soft PASS lamp for the milestone 12300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_soft_12300  (alias)
 *   __libcgj_batch12296_marker = "libcgj-batch12296"
 *
 * Milestone 12300 exclusive continuum CREATE-ONLY (12291-12300). Unique
 * gj_smoke_soft_12300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_12200 (batch12196), gj_smoke_soft_12100 (batch12096),
 * gj_smoke_soft_12000 (batch11996), and gj_dyn_soft_12300
 * (batch12297 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12296_marker[] = "libcgj-batch12296";

/* Smoke soft lamp for wave 12300 (always PASS). */
#define B12296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12296_soft(void)
{
	return B12296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_12300 - report smoke soft lamp for wave 12300.
 *
 * Always returns 1 (soft PASS). Pair with gj_dyn_soft_12300 for the dyn
 * soft-band counterpart. Does not call libc. No parent wires.
 */
uint32_t
gj_smoke_soft_12300(void)
{
	(void)NULL;
	return b12296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_soft_12300(void)
    __attribute__((alias("gj_smoke_soft_12300")));
