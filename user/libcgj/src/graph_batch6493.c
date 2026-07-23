/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6493: static smoke green gate (wave 6500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_6500(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 6500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_6500  (alias)
 *   __libcgj_batch6493_marker = "libcgj-batch6493"
 *
 * Milestone 6500 exclusive continuum CREATE-ONLY (6491-6500). Unique
 * gj_smoke_green_6500 surface only; no multi-def. Distinct from
 * gj_smoke_green_6400 (batch6393), gj_smoke_green_6300 (batch6293),
 * gj_smoke_green_6200 (batch6193), and gj_dyn_green_6500
 * (batch6494 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6493_marker[] = "libcgj-batch6493";

/* Smoke green lamp for wave 6500 (always PASS). */
#define B6493_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6493_green(void)
{
	return B6493_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_6500 - report static smoke green PASS for wave 6500.
 *
 * Always returns 1 (strict green PASS). Soft compile-time product
 * status tag; not a runtime probe. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_smoke_green_6500(void)
{
	(void)NULL;
	return b6493_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_6500(void)
    __attribute__((alias("gj_smoke_green_6500")));
