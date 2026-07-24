/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27497: dyn soft lamp (wave 27500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_27500(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 27500
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_27500  (alias)
 *   __libcgj_batch27497_marker = "libcgj-batch27497"
 *
 * Milestone 27500 exclusive continuum CREATE-ONLY (27491-27500). Unique
 * gj_dyn_soft_27500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_27400 / gj_dyn_soft_27300 / gj_dyn_soft_26900 and
 * sibling 27500 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27491–27500): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_27500). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27497. */
const char __libcgj_batch27497_marker[] = "libcgj-batch27497";

/* Dyn soft ready lamp for wave 27500 (dyn soft surface present). */
#define B27497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27497_soft(void)
{
	return B27497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_27500 - report dyn soft lamp for wave 27500.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_27500(void)
{
	(void)NULL;
	return b27497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_27500(void)
    __attribute__((alias("gj_dyn_soft_27500")));
