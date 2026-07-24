/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32496: smoke soft lamp (wave 32500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_32500(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 32500
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_32500  (alias)
 *   __libcgj_batch32496_marker = "libcgj-batch32496"
 *
 * Milestone 32500 exclusive continuum CREATE-ONLY (32491-32500). Unique
 * gj_smoke_soft_32500 surface only; no multi-def. Distinct from
 * gj_smoke_soft_32400 / gj_smoke_soft_32300 / gj_smoke_soft_32200 and
 * sibling 32500 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (32491–32500): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_32500). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=32500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 32496. */
const char __libcgj_batch32496_marker[] = "libcgj-batch32496";

/* Smoke soft ready lamp for wave 32500 (static soft surface present). */
#define B32496_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32496_soft(void)
{
	return B32496_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_32500 - report smoke soft lamp for wave 32500.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_32500(void)
{
	(void)NULL;
	return b32496_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_32500(void)
    __attribute__((alias("gj_smoke_soft_32500")));
