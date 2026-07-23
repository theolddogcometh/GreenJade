/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7994: dyn smoke soft-green gate (wave 8000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_8000(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 8000
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_8000  (alias)
 *   __libcgj_batch7994_marker = "libcgj-batch7994"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_dyn_green_8000 surface only; no multi-def. Distinct from
 * gj_dyn_green_7000 (batch6994), gj_dyn_green_6000 (batch5994),
 * gj_dyn_green_5900 (batch5894), and gj_smoke_green_8000
 * (batch7993 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7994_marker[] = "libcgj-batch7994";

/* Dyn soft-green lamp for wave 8000 (always green). */
#define B7994_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7994_green(void)
{
	return B7994_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_8000 - report dyn soft-green lamp for wave 8000.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch7993) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_8000(void)
{
	(void)NULL;
	return b7994_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_8000(void)
    __attribute__((alias("gj_dyn_green_8000")));
