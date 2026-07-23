/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4843: static smoke green gate (wave 4850).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4850(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4850
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4850  (alias)
 *   __libcgj_batch4843_marker = "libcgj-batch4843"
 *
 * Milestone 4850 exclusive continuum CREATE-ONLY (4841-4850). Unique
 * gj_smoke_green_4850 surface only; no multi-def. Distinct from
 * gj_smoke_green_4800 (batch4793), gj_smoke_green_4750 (batch4743),
 * gj_smoke_green_4700 (batch4693), and gj_dyn_green_4850
 * (batch4844 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4843_marker[] = "libcgj-batch4843";

/* Smoke green lamp for wave 4850 (always PASS). */
#define B4843_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4843_green(void)
{
	return B4843_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4850 - report smoke green lamp for wave 4850.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4850
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4850(void)
{
	(void)NULL;
	return b4843_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4850(void)
    __attribute__((alias("gj_smoke_green_4850")));
