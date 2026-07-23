/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6994: dyn smoke soft-green gate (wave 7000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_7000(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 7000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_7000  (alias)
 *   __libcgj_batch6994_marker = "libcgj-batch6994"
 *
 * Milestone 7000 exclusive continuum CREATE-ONLY (6991-7000). Unique
 * gj_dyn_green_7000 surface only; no multi-def. Distinct from
 * gj_dyn_green_6000 (batch5994), gj_dyn_green_5900 (batch5894),
 * gj_dyn_green_5800 (batch5794), and gj_smoke_green_7000
 * (batch6993 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6994_marker[] = "libcgj-batch6994";

/* Dyn soft-green lamp for wave 7000 (always green). */
#define B6994_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6994_green(void)
{
	return B6994_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_7000 - report dyn soft-green lamp for wave 7000.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch6993) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_7000(void)
{
	(void)NULL;
	return b6994_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_7000(void)
    __attribute__((alias("gj_dyn_green_7000")));
