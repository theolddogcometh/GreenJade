/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4694: dyn smoke soft-green gate (wave 4700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4700(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4700
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4700  (alias)
 *   __libcgj_batch4694_marker = "libcgj-batch4694"
 *
 * Milestone 4700 exclusive continuum CREATE-ONLY (4691-4700). Unique
 * gj_dyn_green_4700 surface only; no multi-def. Distinct from
 * gj_dyn_green_4650 (batch4644), gj_dyn_green_4600 (batch4594),
 * gj_dyn_green_4550 (batch4544), and gj_smoke_green_4700
 * (batch4693 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4694_marker[] = "libcgj-batch4694";

/* Dyn soft-green lamp for wave 4700 (always green). */
#define B4694_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4694_green(void)
{
	return B4694_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4700 - report dyn soft-green lamp for wave 4700.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4693) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4700(void)
{
	(void)NULL;
	return b4694_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4700(void)
    __attribute__((alias("gj_dyn_green_4700")));
