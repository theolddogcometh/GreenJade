/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23297: dyn soft lamp (wave 23300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_23300(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 23300
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_23300  (alias)
 *   __libcgj_batch23297_marker = "libcgj-batch23297"
 *
 * Milestone 23300 exclusive continuum CREATE-ONLY (23291-23300). Unique
 * gj_dyn_soft_23300 surface only; no multi-def. Distinct from
 * gj_dyn_soft_23200 / gj_dyn_soft_23100 / gj_dyn_soft_23000 and
 * sibling 23300 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (23291–23300): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_23300). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=23300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23297. */
const char __libcgj_batch23297_marker[] = "libcgj-batch23297";

/* Dyn soft ready lamp for wave 23300 (dyn soft surface present). */
#define B23297_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23297_soft(void)
{
	return B23297_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_23300 - report dyn soft lamp for wave 23300.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_23300(void)
{
	(void)NULL;
	return b23297_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_23300(void)
    __attribute__((alias("gj_dyn_soft_23300")));
