/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4993: static smoke green gate (wave 5000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_5000(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 5000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_5000  (alias)
 *   __libcgj_batch4993_marker = "libcgj-batch4993"
 *
 * Milestone 5000 exclusive continuum CREATE-ONLY (4991-5000). Unique
 * gj_smoke_green_5000 surface only; no multi-def. Distinct from
 * gj_smoke_green_4900 (batch4893), gj_smoke_green_4850 (batch4843),
 * gj_smoke_green_4800 (batch4793), and gj_dyn_green_5000
 * (batch4994 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4993_marker[] = "libcgj-batch4993";

/* Smoke green lamp for wave 5000 (always PASS). */
#define B4993_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4993_green(void)
{
	return B4993_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_5000 - report smoke green lamp for wave 5000.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_5000
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_5000(void)
{
	(void)NULL;
	return b4993_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_5000(void)
    __attribute__((alias("gj_smoke_green_5000")));
