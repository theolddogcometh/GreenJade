/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26097: dyn soft lamp (wave 26100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_26100(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 26100
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_26100  (alias)
 *   __libcgj_batch26097_marker = "libcgj-batch26097"
 *
 * Milestone 26100 exclusive continuum CREATE-ONLY (26091-26100). Unique
 * gj_dyn_soft_26100 surface only; no multi-def. Distinct from
 * gj_dyn_soft_26000 / gj_dyn_soft_25900 / gj_dyn_soft_25800 and
 * sibling 26100 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26091–26100): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_26100). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26097. */
const char __libcgj_batch26097_marker[] = "libcgj-batch26097";

/* Dyn soft ready lamp for wave 26100 (dyn soft surface present). */
#define B26097_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26097_soft(void)
{
	return B26097_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_26100 - report dyn soft lamp for wave 26100.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_26100(void)
{
	(void)NULL;
	return b26097_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_26100(void)
    __attribute__((alias("gj_dyn_soft_26100")));
