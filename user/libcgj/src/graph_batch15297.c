/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15297: dyn soft lamp (wave 15300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_15300(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 15300
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_15300  (alias)
 *   __libcgj_batch15297_marker = "libcgj-batch15297"
 *
 * Milestone 15300 exclusive continuum CREATE-ONLY (15291-15300). Unique
 * gj_dyn_soft_15300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_15200 / gj_dyn_soft_15100 / gj_dyn_soft_15000 and
 * sibling 15300 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (15291–15300): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_15300). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=15300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15297. */
const char __libcgj_batch15297_marker[] = "libcgj-batch15297";

/* Dyn soft ready lamp for wave 15300 (dyn soft surface present). */
#define B15297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15297_soft(void)
{
	return B15297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_15300 - report dyn soft lamp for wave 15300.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_15300(void)
{
	(void)NULL;
	return b15297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_15300(void)
    __attribute__((alias("gj_dyn_soft_15300")));
