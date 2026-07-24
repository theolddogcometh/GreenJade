/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44797: dyn soft lamp (wave 44800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_44800(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 44800
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_44800  (alias)
 *   __libcgj_batch44797_marker = "libcgj-batch44797"
 *
 * Milestone 44800 exclusive continuum CREATE-ONLY (44791-44800). Unique
 * gj_dyn_soft_44800 surface only; no multi-def. Distinct from
 * gj_dyn_soft_44700 / gj_dyn_soft_44600 / gj_dyn_soft_44500 and
 * sibling 44800 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44791–44800): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_44800). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44797. */
const char __libcgj_batch44797_marker[] = "libcgj-batch44797";

/* Dyn soft ready lamp for wave 44800 (dyn soft surface present). */
#define B44797_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44797_soft(void)
{
	return B44797_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_44800 - report dyn soft lamp for wave 44800.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_44800(void)
{
	(void)NULL;
	return b44797_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_44800(void)
    __attribute__((alias("gj_dyn_soft_44800")));
