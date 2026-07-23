/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5294: dyn smoke soft-green gate (wave 5300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_5300(void);
 *     - Returns 1 (dyn-smoke soft-green lamp for the milestone 5300
 *       continuum). Soft compile-time product status tag.
 *   uint32_t __gj_dyn_green_5300  (alias)
 *   __libcgj_batch5294_marker = "libcgj-batch5294"
 *
 * Milestone 5300 exclusive continuum CREATE-ONLY (5291-5300). Unique
 * gj_dyn_green_5300 surface only; no multi-def. Distinct from
 * gj_dyn_green_5200 (batch5194), gj_dyn_green_5100 (batch5094),
 * gj_dyn_green_5000 (batch4994), and gj_smoke_green_5300
 * (batch5293 exact PASS). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5294_marker[] = "libcgj-batch5294";

/* Dyn soft-green lamp for wave 5300 (always green). */
#define B5294_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5294_green(void)
{
	return B5294_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_5300 - report dyn soft-green lamp for wave 5300.
 *
 * Always returns 1 (soft-green). Distinct from static smoke green
 * (batch5293) and paired dyn gates. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_dyn_green_5300(void)
{
	(void)NULL;
	return b5294_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_5300(void)
    __attribute__((alias("gj_dyn_green_5300")));
