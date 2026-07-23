/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4944: dyn smoke soft-green gate (wave 4950).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4950(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4950
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4950  (alias)
 *   __libcgj_batch4944_marker = "libcgj-batch4944"
 *
 * Milestone 4950 exclusive continuum CREATE-ONLY (4941-4950). Unique
 * gj_dyn_green_4950 surface only; no multi-def. Distinct from
 * gj_dyn_green_4900 (batch4894), gj_dyn_green_4850 (batch4844),
 * gj_dyn_green_4800 (batch4794), and gj_smoke_green_4950
 * (batch4943 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4944_marker[] = "libcgj-batch4944";

/* Dyn soft-green lamp for wave 4950 (always green). */
#define B4944_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4944_green(void)
{
	return B4944_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4950 - report dyn soft-green lamp for wave 4950.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4943) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4950(void)
{
	(void)NULL;
	return b4944_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4950(void)
    __attribute__((alias("gj_dyn_green_4950")));
