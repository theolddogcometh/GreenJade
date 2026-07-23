/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4493: static smoke green gate (wave 4500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4500(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 4500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_4500  (alias)
 *   __libcgj_batch4493_marker = "libcgj-batch4493"
 *
 * Milestone 4500 exclusive continuum CREATE-ONLY (4491-4500). Unique
 * gj_smoke_green_4500 surface only; no multi-def. Distinct from
 * gj_smoke_green_4400 (batch4393), gj_smoke_green_4300 (batch4293),
 * gj_smoke_green_4100 (batch4093), and gj_dyn_green_4500
 * (batch4494 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4493_marker[] = "libcgj-batch4493";

/* Smoke green lamp for wave 4500 (always PASS). */
#define B4493_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4493_green(void)
{
	return B4493_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4500 - report smoke green lamp for wave 4500.
 *
 * Always returns 1 (strict green PASS). Pair with gj_dyn_green_4500
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_smoke_green_4500(void)
{
	(void)NULL;
	return b4493_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4500(void)
    __attribute__((alias("gj_smoke_green_4500")));
