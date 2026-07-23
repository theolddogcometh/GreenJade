/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4394: dyn smoke soft-green gate (wave 4400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4400(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4400
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4400  (alias)
 *   __libcgj_batch4394_marker = "libcgj-batch4394"
 *
 * Milestone 4400 exclusive continuum CREATE-ONLY (4391-4400). Unique
 * gj_dyn_green_4400 surface only; no multi-def. Distinct from
 * gj_dyn_green_4300 (batch4294), gj_dyn_green_4100 (batch4094),
 * gj_dyn_green_3950 (batch3944), and gj_smoke_green_4400
 * (batch4393 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4394_marker[] = "libcgj-batch4394";

/* Dyn soft-green lamp for wave 4400 (always green). */
#define B4394_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4394_green(void)
{
	return B4394_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4400 - report dyn soft-green lamp for wave 4400.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4393) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4400(void)
{
	(void)NULL;
	return b4394_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4400(void)
    __attribute__((alias("gj_dyn_green_4400")));
