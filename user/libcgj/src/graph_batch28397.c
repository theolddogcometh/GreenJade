/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28397: dyn soft lamp (wave 28400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_28400(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 28400
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_28400  (alias)
 *   __libcgj_batch28397_marker = "libcgj-batch28397"
 *
 * Milestone 28400 exclusive continuum CREATE-ONLY (28391-28400). Unique
 * gj_dyn_soft_28400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_28300 / gj_dyn_soft_28200 / gj_dyn_soft_28100 and
 * sibling 28400 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28391–28400): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_28400). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28397. */
const char __libcgj_batch28397_marker[] = "libcgj-batch28397";

/* Dyn soft ready lamp for wave 28400 (dyn soft surface present). */
#define B28397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28397_soft(void)
{
	return B28397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_28400 - report dyn soft lamp for wave 28400.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_28400(void)
{
	(void)NULL;
	return b28397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_28400(void)
    __attribute__((alias("gj_dyn_soft_28400")));
