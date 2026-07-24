/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32497: dyn soft lamp (wave 32500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_32500(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 32500
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_32500  (alias)
 *   __libcgj_batch32497_marker = "libcgj-batch32497"
 *
 * Milestone 32500 exclusive continuum CREATE-ONLY (32491-32500). Unique
 * gj_dyn_soft_32500 surface only; no multi-def. Distinct from
 * gj_dyn_soft_32400 / gj_dyn_soft_32300 / gj_dyn_soft_32200 and
 * sibling 32500 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (32491–32500): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_32500). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=32500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 32497. */
const char __libcgj_batch32497_marker[] = "libcgj-batch32497";

/* Dyn soft ready lamp for wave 32500 (dyn soft surface present). */
#define B32497_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32497_soft(void)
{
	return B32497_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_32500 - report dyn soft lamp for wave 32500.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_32500(void)
{
	(void)NULL;
	return b32497_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_32500(void)
    __attribute__((alias("gj_dyn_soft_32500")));
