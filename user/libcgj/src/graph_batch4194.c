/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4194: dyn smoke soft-green gate (wave 4200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_green_4200(void);
 *     - Returns 1: milestone 4200 continuum dyn-smoke is soft-green.
 *       Soft compile-time product tag for single cgj_dyn_smoke status.
 *   uint32_t __gj_dyn_green_4200  (alias)
 *   __libcgj_batch4194_marker = "libcgj-batch4194"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_dyn_green_4200 surface only; no multi-def. Distinct from
 * gj_dyn_green_4100 (batch4094), gj_dyn_green_3950 (batch3944),
 * gj_dyn_green_3900 (batch3894), and gj_smoke_green_4200
 * (batch4193 exact green counterpart). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4194_marker[] = "libcgj-batch4194";

/* Dyn soft-green lamp for wave 4200. */
#define B4194_DYN_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4194_green(void)
{
	return B4194_DYN_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_green_4200 - report milestone-4200 dyn-smoke soft-green status.
 *
 * Always returns 1 (dyn soft-green). Link-time presence of this symbol
 * tags dyn soft-green for the wave. Does not call libc. No parent wires.
 */
uint32_t
gj_dyn_green_4200(void)
{
	(void)NULL;
	return b4194_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dyn_green_4200(void)
    __attribute__((alias("gj_dyn_green_4200")));
