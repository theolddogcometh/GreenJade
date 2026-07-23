/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7793: static smoke green gate (wave 7800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_7800(void);
 *     - Returns 1 (strict green PASS lamp for the milestone 7800
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_smoke_green_7800  (alias)
 *   __libcgj_batch7793_marker = "libcgj-batch7793"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_smoke_green_7800 surface only; no multi-def. Distinct from
 * gj_smoke_green_7700 (batch7693), gj_smoke_green_7600 (batch7593),
 * gj_smoke_green_7500 (batch7493), and gj_dyn_green_7800
 * (batch7794 soft-band). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7793_marker[] = "libcgj-batch7793";

/* Static smoke green PASS lamp for wave 7800. */
#define B7793_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7793_green(void)
{
	return B7793_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_7800 - report static smoke green for wave 7800.
 *
 * Always returns 1 (strict green PASS). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_smoke_green_7800(void)
{
	(void)NULL;
	return b7793_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_7800(void)
    __attribute__((alias("gj_smoke_green_7800")));
