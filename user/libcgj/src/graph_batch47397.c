/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47397: dyn soft lamp (wave 47400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_47400(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 47400
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_47400  (alias)
 *   __libcgj_batch47397_marker = "libcgj-batch47397"
 *
 * Milestone 47400 exclusive continuum CREATE-ONLY (47391-47400). Unique
 * gj_dyn_soft_47400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_47300 / gj_dyn_soft_47400 / gj_dyn_soft_47100 and
 * sibling 47400 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (47391–47400): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_47400). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 47397. */
const char __libcgj_batch47397_marker[] = "libcgj-batch47397";

/* Dyn soft ready lamp for wave 47400 (dyn soft surface present). */
#define B47397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47397_soft(void)
{
	return B47397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_47400 - report dyn soft lamp for wave 47400.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_47400(void)
{
	(void)NULL;
	return b47397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_47400(void)
    __attribute__((alias("gj_dyn_soft_47400")));
