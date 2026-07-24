/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32296: smoke soft lamp (wave 32300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_32300(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 32300
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_32300  (alias)
 *   __libcgj_batch32296_marker = "libcgj-batch32296"
 *
 * Milestone 32300 exclusive continuum CREATE-ONLY (32291-32300). Unique
 * gj_smoke_soft_32300 surface only; no multi-def. Distinct from
 * gj_smoke_soft_32200 / gj_smoke_soft_32100 / gj_smoke_soft_32000 and
 * sibling 32300 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (32291–32300): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_32300). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=32300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 32296. */
const char __libcgj_batch32296_marker[] = "libcgj-batch32296";

/* Smoke soft ready lamp for wave 32300 (static soft surface present). */
#define B32296_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32296_soft(void)
{
	return B32296_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_32300 - report smoke soft lamp for wave 32300.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_32300(void)
{
	(void)NULL;
	return b32296_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_32300(void)
    __attribute__((alias("gj_smoke_soft_32300")));
