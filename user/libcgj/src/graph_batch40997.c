/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40997: dyn soft lamp (wave 41000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_41000(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 41000
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_41000  (alias)
 *   __libcgj_batch40997_marker = "libcgj-batch40997"
 *
 * Milestone 41000 exclusive continuum CREATE-ONLY (40991-41000). Unique
 * gj_dyn_soft_41000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_40900 / gj_dyn_soft_40800 / gj_dyn_soft_40700 and
 * sibling 41000 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (40991–41000): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_41000). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=41000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40997. */
const char __libcgj_batch40997_marker[] = "libcgj-batch40997";

/* Dyn soft ready lamp for wave 41000 (dyn soft surface present). */
#define B40997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40997_soft(void)
{
	return B40997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_41000 - report dyn soft lamp for wave 41000.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_41000(void)
{
	(void)NULL;
	return b40997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_41000(void)
    __attribute__((alias("gj_dyn_soft_41000")));
