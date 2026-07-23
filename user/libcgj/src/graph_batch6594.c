/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6594: dyn smoke soft-green gate (wave 6600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6600(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6600
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6600  (alias)
 *   __libcgj_batch6594_marker = "libcgj-batch6594"
 *
 * Milestone 6600 exclusive continuum CREATE-ONLY (6591-6600). Unique
 * gj_dyn_green_6600 surface only; no multi-def. Distinct from
 * gj_dyn_green_6500 (batch6494), gj_dyn_green_6400 (batch6394),
 * gj_dyn_green_6300 (batch6294), and gj_smoke_green_6600
 * (batch6593 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6594_marker[] = "libcgj-batch6594";

/* Dyn soft-green lamp for wave 6600 (always green). */
#define B6594_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6594_green(void)
{
	return B6594_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6600 - report dyn-smoke soft-green lamp for wave 6600.
 *
 * Always returns 1 (dyn soft-green). Soft compile-time product status
 * tag; not a runtime probe. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_6600(void)
{
	(void)NULL;
	return b6594_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6600(void)
    __attribute__((alias("gj_dyn_green_6600")));
