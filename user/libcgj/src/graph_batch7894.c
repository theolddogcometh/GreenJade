/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7894: dyn smoke soft-green gate (wave 7900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7900(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7900
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7900  (alias)
 *   __libcgj_batch7894_marker = "libcgj-batch7894"
 *
 * Milestone 7900 exclusive continuum CREATE-ONLY (7891-7900). Unique
 * gj_dyn_green_7900 surface only; no multi-def. Distinct from
 * gj_dyn_green_7800 (batch7794), gj_dyn_green_7700 (batch7694),
 * gj_dyn_green_7600 (batch7594), and gj_smoke_green_7900
 * (batch7893 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7894_marker[] = "libcgj-batch7894";

/* Dyn-smoke soft-green lamp for wave 7900. */
#define B7894_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7894_green(void)
{
	return B7894_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7900 - report dyn-smoke soft-green for wave 7900.
 *
 * Always returns 1 (dyn soft-green). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_dyn_green_7900(void)
{
	(void)NULL;
	return b7894_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7900(void)
    __attribute__((alias("gj_dyn_green_7900")));
