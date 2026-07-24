/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30197: dyn soft lamp (wave 30200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_30200(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 30200
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_30200  (alias)
 *   __libcgj_batch30197_marker = "libcgj-batch30197"
 *
 * Milestone 30200 exclusive continuum CREATE-ONLY (30191-30200). Unique
 * gj_dyn_soft_30200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_30100 / gj_dyn_soft_30000 / gj_dyn_soft_29900 and
 * sibling 30200 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30191–30200): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_30200). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30197. */
const char __libcgj_batch30197_marker[] = "libcgj-batch30197";

/* Dyn soft ready lamp for wave 30200 (dyn soft surface present). */
#define B30197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30197_soft(void)
{
	return B30197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_30200 - report dyn soft lamp for wave 30200.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_30200(void)
{
	(void)NULL;
	return b30197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_30200(void)
    __attribute__((alias("gj_dyn_soft_30200")));
