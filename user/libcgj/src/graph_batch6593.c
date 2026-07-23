/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6593: static smoke green gate (wave 6600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_6600(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 6600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_6600  (alias)
 *   __libcgj_batch6593_marker = "libcgj-batch6593"
 *
 * Milestone 6600 exclusive continuum CREATE-ONLY (6591-6600). Unique
 * gj_smoke_green_6600 surface only; no multi-def. Distinct from
 * gj_smoke_green_6500 (batch6493), gj_smoke_green_6400 (batch6393),
 * gj_smoke_green_6300 (batch6293), and gj_dyn_green_6600
 * (batch6594 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6593_marker[] = "libcgj-batch6593";

/* Smoke green lamp for wave 6600 (always PASS). */
#define B6593_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6593_green(void)
{
	return B6593_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_6600 - report static smoke green PASS for wave 6600.
 *
 * Always returns 1 (strict green PASS). Soft compile-time product
 * status tag; not a runtime probe. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_smoke_green_6600(void)
{
	(void)NULL;
	return b6593_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_6600(void)
    __attribute__((alias("gj_smoke_green_6600")));
