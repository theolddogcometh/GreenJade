/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26096: smoke soft lamp (wave 26100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_26100(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 26100
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_26100  (alias)
 *   __libcgj_batch26096_marker = "libcgj-batch26096"
 *
 * Milestone 26100 exclusive continuum CREATE-ONLY (26091-26100). Unique
 * gj_smoke_soft_26100 surface only; no multi-def. Distinct from
 * gj_smoke_soft_26000 / gj_smoke_soft_25900 / gj_smoke_soft_25800 and
 * sibling 26100 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26091–26100): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_26100). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26096. */
const char __libcgj_batch26096_marker[] = "libcgj-batch26096";

/* Smoke soft ready lamp for wave 26100 (static soft surface present). */
#define B26096_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26096_soft(void)
{
	return B26096_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_26100 - report smoke soft lamp for wave 26100.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_26100(void)
{
	(void)NULL;
	return b26096_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_26100(void)
    __attribute__((alias("gj_smoke_soft_26100")));
