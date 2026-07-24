/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26897: dyn soft lamp (wave 26900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_26900(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 26900
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_26900  (alias)
 *   __libcgj_batch26897_marker = "libcgj-batch26897"
 *
 * Milestone 26900 exclusive continuum CREATE-ONLY (26891-26900). Unique
 * gj_dyn_soft_26900 surface only; no multi-def. Distinct from
 * gj_dyn_soft_26800 / gj_dyn_soft_26700 / gj_dyn_soft_26600 and
 * sibling 26900 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26891–26900): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_26900). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26897. */
const char __libcgj_batch26897_marker[] = "libcgj-batch26897";

/* Dyn soft ready lamp for wave 26900 (dyn soft surface present). */
#define B26897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26897_soft(void)
{
	return B26897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_26900 - report dyn soft lamp for wave 26900.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_26900(void)
{
	(void)NULL;
	return b26897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_26900(void)
    __attribute__((alias("gj_dyn_soft_26900")));
