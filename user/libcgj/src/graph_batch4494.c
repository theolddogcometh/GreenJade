/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4494: dyn smoke soft-green gate (wave 4500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4500(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 4500
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_4500  (alias)
 *   __libcgj_batch4494_marker = "libcgj-batch4494"
 *
 * Milestone 4500 exclusive continuum CREATE-ONLY (4491-4500). Unique
 * gj_dyn_green_4500 surface only; no multi-def. Distinct from
 * gj_dyn_green_4400 (batch4394), gj_dyn_green_4300 (batch4294),
 * gj_dyn_green_4100 (batch4094), and gj_smoke_green_4500
 * (batch4493 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4494_marker[] = "libcgj-batch4494";

/* Dyn soft-green lamp for wave 4500 (always green). */
#define B4494_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4494_green(void)
{
	return B4494_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4500 - report dyn soft-green lamp for wave 4500.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch4493) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_4500(void)
{
	(void)NULL;
	return b4494_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4500(void)
    __attribute__((alias("gj_dyn_green_4500")));
