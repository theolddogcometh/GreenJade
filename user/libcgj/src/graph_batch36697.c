/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36697: dyn soft lamp (wave 36700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_36700(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 36700
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_36700  (alias)
 *   __libcgj_batch36697_marker = "libcgj-batch36697"
 *
 * Milestone 36700 exclusive continuum CREATE-ONLY (36691-36700). Unique
 * gj_dyn_soft_36700 surface only; no multi-def. Distinct from
 * gj_dyn_soft_36600 / gj_dyn_soft_36500 / gj_dyn_soft_36400 and
 * sibling 36700 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36691–36700): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_36700). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36700 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36697. */
const char __libcgj_batch36697_marker[] = "libcgj-batch36697";

/* Dyn soft ready lamp for wave 36700 (dyn soft surface present). */
#define B36697_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36697_soft(void)
{
	return B36697_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_36700 - report dyn soft lamp for wave 36700.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_36700(void)
{
	(void)NULL;
	return b36697_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_36700(void)
    __attribute__((alias("gj_dyn_soft_36700")));
