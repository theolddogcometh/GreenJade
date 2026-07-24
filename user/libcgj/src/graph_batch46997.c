/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46997: dyn soft lamp (wave 47000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_47000(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 47000
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_47000  (alias)
 *   __libcgj_batch46997_marker = "libcgj-batch46997"
 *
 * Milestone 47000 exclusive continuum CREATE-ONLY (46991-47000). Unique
 * gj_dyn_soft_47000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_46900 / gj_dyn_soft_46800 / gj_dyn_soft_46700 and
 * sibling 47000 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (46991–47000): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_47000). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 46997. */
const char __libcgj_batch46997_marker[] = "libcgj-batch46997";

/* Dyn soft ready lamp for wave 47000 (dyn soft surface present). */
#define B46997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46997_soft(void)
{
	return B46997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_47000 - report dyn soft lamp for wave 47000.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_47000(void)
{
	(void)NULL;
	return b46997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_47000(void)
    __attribute__((alias("gj_dyn_soft_47000")));
