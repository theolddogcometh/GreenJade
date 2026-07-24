/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30097: dyn soft lamp (wave 30100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_30100(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 30100
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_30100  (alias)
 *   __libcgj_batch30097_marker = "libcgj-batch30097"
 *
 * Milestone 30100 exclusive continuum CREATE-ONLY (30091-30100). Unique
 * gj_dyn_soft_30100 surface only; no multi-def. Distinct from
 * gj_dyn_soft_30000 / gj_dyn_soft_29900 / gj_dyn_soft_29800 and
 * sibling 30100 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30091–30100): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_30100). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30097. */
const char __libcgj_batch30097_marker[] = "libcgj-batch30097";

/* Dyn soft ready lamp for wave 30100 (dyn soft surface present). */
#define B30097_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30097_soft(void)
{
	return B30097_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_30100 - report dyn soft lamp for wave 30100.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_30100(void)
{
	(void)NULL;
	return b30097_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_30100(void)
    __attribute__((alias("gj_dyn_soft_30100")));
