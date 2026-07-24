/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45997: dyn soft lamp (wave 46000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_46000(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 46000
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_46000  (alias)
 *   __libcgj_batch45997_marker = "libcgj-batch45997"
 *
 * Milestone 46000 exclusive continuum CREATE-ONLY (45991-46000). Unique
 * gj_dyn_soft_46000 surface only; no multi-def. Distinct from
 * gj_dyn_soft_45900 / gj_dyn_soft_45800 / gj_dyn_soft_45700 and
 * sibling 46000 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45991–46000): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_46000). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=46000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45997. */
const char __libcgj_batch45997_marker[] = "libcgj-batch45997";

/* Dyn soft ready lamp for wave 46000 (dyn soft surface present). */
#define B45997_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45997_soft(void)
{
	return B45997_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_46000 - report dyn soft lamp for wave 46000.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_46000(void)
{
	(void)NULL;
	return b45997_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_46000(void)
    __attribute__((alias("gj_dyn_soft_46000")));
