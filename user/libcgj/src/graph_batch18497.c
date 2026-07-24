/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18497: dyn soft lamp (wave 18500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_18500(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 18500
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_18500  (alias)
 *   __libcgj_batch18497_marker = "libcgj-batch18497"
 *
 * Milestone 18500 exclusive continuum CREATE-ONLY (18491-18500). Unique
 * gj_dyn_soft_18500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_18300 / gj_dyn_soft_18200 / gj_dyn_soft_18100 and
 * sibling 18500 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (18491–18500): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_18500). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=18500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18497. */
const char __libcgj_batch18497_marker[] = "libcgj-batch18497";

/* Dyn soft ready lamp for wave 18500 (dyn soft surface present). */
#define B18497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18497_soft(void)
{
	return B18497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_18500 - report dyn soft lamp for wave 18500.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_18500(void)
{
	(void)NULL;
	return b18497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_18500(void)
    __attribute__((alias("gj_dyn_soft_18500")));
