/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21297: dyn soft lamp (wave 21300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_21300(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 21300
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_21300  (alias)
 *   __libcgj_batch21297_marker = "libcgj-batch21297"
 *
 * Milestone 21300 exclusive continuum CREATE-ONLY (21291-21300). Unique
 * gj_dyn_soft_21300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_21200 / gj_dyn_soft_21100 / gj_dyn_soft_21000 and
 * sibling 21300 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21291–21300): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_21300). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21297. */
const char __libcgj_batch21297_marker[] = "libcgj-batch21297";

/* Dyn soft ready lamp for wave 21300 (dyn soft surface present). */
#define B21297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21297_soft(void)
{
	return B21297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_21300 - report dyn soft lamp for wave 21300.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_21300(void)
{
	(void)NULL;
	return b21297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_21300(void)
    __attribute__((alias("gj_dyn_soft_21300")));
