/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28897: dyn soft lamp (wave 28900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_28900(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 28900
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_28900  (alias)
 *   __libcgj_batch28897_marker = "libcgj-batch28897"
 *
 * Milestone 28900 exclusive continuum CREATE-ONLY (28891-28900). Unique
 * gj_dyn_soft_28900 surface only; no multi-def. Distinct from
 * gj_dyn_soft_28800 / gj_dyn_soft_28700 / gj_dyn_soft_28600 and
 * sibling 28900 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28891–28900): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_28900). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28897. */
const char __libcgj_batch28897_marker[] = "libcgj-batch28897";

/* Dyn soft ready lamp for wave 28900 (dyn soft surface present). */
#define B28897_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28897_soft(void)
{
	return B28897_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_28900 - report dyn soft lamp for wave 28900.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_28900(void)
{
	(void)NULL;
	return b28897_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_28900(void)
    __attribute__((alias("gj_dyn_soft_28900")));
