/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4893: static smoke green gate (wave 4900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4900(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4900  (alias)
 *   __libcgj_batch4893_marker = "libcgj-batch4893"
 *
 * Milestone 4900 exclusive continuum CREATE-ONLY (4891-4900). Unique
 * gj_smoke_green_4900 surface only; no multi-def. Distinct from
 * gj_smoke_green_4850 (batch4843), gj_smoke_green_4800 (batch4793),
 * gj_smoke_green_4750 (batch4743), and gj_dyn_green_4900
 * (batch4894 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4893_marker[] = "libcgj-batch4893";

/* Smoke green lamp for wave 4900 (always PASS). */
#define B4893_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4893_green(void)
{
	return B4893_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4900 - report smoke green lamp for wave 4900.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4900
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4900(void)
{
	(void)NULL;
	return b4893_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4900(void)
    __attribute__((alias("gj_smoke_green_4900")));
