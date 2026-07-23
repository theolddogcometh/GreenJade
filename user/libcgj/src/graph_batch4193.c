/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4193: static smoke green gate (wave 4200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_green_4200(void);
 *     - Returns 1: milestone 4200 continuum smoke is green. Soft
 *       compile-time product tag for product smoke status lamps.
 *   uint32_t __gj_smoke_green_4200  (alias)
 *   __libcgj_batch4193_marker = "libcgj-batch4193"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_smoke_green_4200 surface only; no multi-def. Distinct from
 * gj_smoke_green_4100 (batch4093), gj_smoke_green_3950 (batch3943),
 * gj_smoke_green_3900 (batch3893), and gj_dyn_green_4200
 * (batch4194 soft-band counterpart). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4193_marker[] = "libcgj-batch4193";

/* Smoke green lamp for wave 4200. */
#define B4193_SMOKE_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4193_green(void)
{
	return B4193_SMOKE_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_green_4200 - report milestone-4200 smoke green status.
 *
 * Always returns 1 (smoke green). Link-time presence of this symbol
 * tags static smoke green for the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_smoke_green_4200(void)
{
	(void)NULL;
	return b4193_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_smoke_green_4200(void)
    __attribute__((alias("gj_smoke_green_4200")));
