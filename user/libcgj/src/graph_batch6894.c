/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6894: dyn smoke soft-green gate (wave 6900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_6900(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 6900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_6900  (alias)
 *   __libcgj_batch6894_marker = "libcgj-batch6894"
 *
 * Milestone 6900 exclusive continuum CREATE-ONLY (6891-6900). Unique
 * gj_dyn_green_6900 surface only; no multi-def. Distinct from
 * gj_dyn_green_6800 (batch6794), gj_dyn_green_6700 (batch6694),
 * gj_dyn_green_6600 (batch6594), and gj_smoke_green_6900
 * (batch6893 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6894_marker[] = "libcgj-batch6894";

/* Dyn soft-green lamp for wave 6900 (always green). */
#define B6894_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6894_green(void)
{
	return B6894_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_6900 - report dyn-smoke soft-green lamp for wave 6900.
 *
 * Always returns 1 (dyn soft-green). Soft compile-time product status
 * tag; not a runtime probe. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_6900(void)
{
	(void)NULL;
	return b6894_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_6900(void)
    __attribute__((alias("gj_dyn_green_6900")));
