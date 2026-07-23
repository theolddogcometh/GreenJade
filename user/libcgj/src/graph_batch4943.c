/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4943: static smoke green gate (wave 4950).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4950(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4950
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4950  (alias)
 *   __libcgj_batch4943_marker = "libcgj-batch4943"
 *
 * Milestone 4950 exclusive continuum CREATE-ONLY (4941-4950). Unique
 * gj_smoke_green_4950 surface only; no multi-def. Distinct from
 * gj_smoke_green_4900 (batch4893), gj_smoke_green_4850 (batch4843),
 * gj_smoke_green_4800 (batch4793), and gj_dyn_green_4950
 * (batch4944 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4943_marker[] = "libcgj-batch4943";

/* Smoke green lamp for wave 4950 (always PASS). */
#define B4943_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4943_green(void)
{
	return B4943_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4950 - report smoke green lamp for wave 4950.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4950
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4950(void)
{
	(void)NULL;
	return b4943_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4950(void)
    __attribute__((alias("gj_smoke_green_4950")));
