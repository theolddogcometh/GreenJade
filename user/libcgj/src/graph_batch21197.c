/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21197: dyn soft lamp (wave 21200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_21200(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 21200
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_21200  (alias)
 *   __libcgj_batch21197_marker = "libcgj-batch21197"
 *
 * Milestone 21200 exclusive continuum CREATE-ONLY (21191-21200). Unique
 * gj_dyn_soft_21200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_21100 / gj_dyn_soft_21000 / gj_dyn_soft_20900 and
 * sibling 21200 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21191–21200): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_21200). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21197. */
const char __libcgj_batch21197_marker[] = "libcgj-batch21197";

/* Dyn soft ready lamp for wave 21200 (dyn soft surface present). */
#define B21197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21197_soft(void)
{
	return B21197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_21200 - report dyn soft lamp for wave 21200.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_21200(void)
{
	(void)NULL;
	return b21197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_21200(void)
    __attribute__((alias("gj_dyn_soft_21200")));
