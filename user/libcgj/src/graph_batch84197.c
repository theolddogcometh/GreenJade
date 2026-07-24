/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch84197: dyn soft lamp (wave 84200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_84200(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 84200
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_84200  (alias)
 *   __libcgj_batch84197_marker = "libcgj-batch84197"
 *
 * Milestone 84200 exclusive continuum CREATE-ONLY (25891-84200). Unique
 * gj_dyn_soft_84200 surface only; no multi-def. Distinct from
 * gj_dyn_soft_25800 / gj_dyn_soft_25700 / gj_dyn_soft_25600 and
 * sibling 84200 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–84200): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_84200). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=84200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 84197. */
const char __libcgj_batch84197_marker[] = "libcgj-batch84197";

/* Dyn soft ready lamp for wave 84200 (dyn soft surface present). */
#define B84197_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b84197_soft(void)
{
	return B84197_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_84200 - report dyn soft lamp for wave 84200.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_84200(void)
{
	(void)NULL;
	return b84197_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_84200(void)
    __attribute__((alias("gj_dyn_soft_84200")));
