/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24297: dyn soft lamp (wave 24300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_24300(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 24300
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_24300  (alias)
 *   __libcgj_batch24297_marker = "libcgj-batch24297"
 *
 * Milestone 24300 exclusive continuum CREATE-ONLY (24291-24300). Unique
 * gj_dyn_soft_24300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_24200 / gj_dyn_soft_24100 / gj_dyn_soft_24000 and
 * sibling 24300 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (24291–24300): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_24300). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=24300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24297. */
const char __libcgj_batch24297_marker[] = "libcgj-batch24297";

/* Dyn soft ready lamp for wave 24300 (dyn soft surface present). */
#define B24297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24297_soft(void)
{
	return B24297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_24300 - report dyn soft lamp for wave 24300.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_24300(void)
{
	(void)NULL;
	return b24297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_24300(void)
    __attribute__((alias("gj_dyn_soft_24300")));
