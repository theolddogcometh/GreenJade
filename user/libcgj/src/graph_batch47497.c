/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47497: dyn soft lamp (wave 47500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_47500(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 47500
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_47500  (alias)
 *   __libcgj_batch47497_marker = "libcgj-batch47497"
 *
 * Milestone 47500 exclusive continuum CREATE-ONLY (47491-47500). Unique
 * gj_dyn_soft_47500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_47400 / gj_dyn_soft_47300 / gj_dyn_soft_47200 and
 * sibling 47500 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (47491–47500): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_47500). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 47497. */
const char __libcgj_batch47497_marker[] = "libcgj-batch47497";

/* Dyn soft ready lamp for wave 47500 (dyn soft surface present). */
#define B47497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47497_soft(void)
{
	return B47497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_47500 - report dyn soft lamp for wave 47500.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_47500(void)
{
	(void)NULL;
	return b47497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_47500(void)
    __attribute__((alias("gj_dyn_soft_47500")));
