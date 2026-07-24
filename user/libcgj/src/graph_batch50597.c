/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50597: dyn soft lamp (wave 50600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_50600(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 50600
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_50600  (alias)
 *   __libcgj_batch50597_marker = "libcgj-batch50597"
 *
 * Milestone 50600 exclusive continuum CREATE-ONLY (50591-50600). Unique
 * gj_dyn_soft_50600 surface only; no multi-def. Distinct from
 * gj_dyn_soft_50500 / gj_dyn_soft_50400 / gj_dyn_soft_50300 and
 * sibling 50600 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50591–50600): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_50600). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50597. */
const char __libcgj_batch50597_marker[] = "libcgj-batch50597";

/* Dyn soft ready lamp for wave 50600 (dyn soft surface present). */
#define B50597_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50597_soft(void)
{
	return B50597_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_50600 - report dyn soft lamp for wave 50600.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_50600(void)
{
	(void)NULL;
	return b50597_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_50600(void)
    __attribute__((alias("gj_dyn_soft_50600")));
