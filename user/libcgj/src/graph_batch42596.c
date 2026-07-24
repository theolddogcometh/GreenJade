/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42596: smoke soft lamp (wave 42600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_42600(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 42600
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_42600  (alias)
 *   __libcgj_batch42596_marker = "libcgj-batch42596"
 *
 * Milestone 42600 exclusive continuum CREATE-ONLY (42591-42600). Unique
 * gj_smoke_soft_42600 surface only; no multi-def. Distinct from
 * gj_smoke_soft_42300 / gj_smoke_soft_42200 / gj_smoke_soft_42100 and
 * sibling 42600 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42591–42600): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_42600). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42596. */
const char __libcgj_batch42596_marker[] = "libcgj-batch42596";

/* Smoke soft ready lamp for wave 42600 (static soft surface present). */
#define B42596_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42596_soft(void)
{
	return B42596_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_42600 - report smoke soft lamp for wave 42600.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_42600(void)
{
	(void)NULL;
	return b42596_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_42600(void)
    __attribute__((alias("gj_smoke_soft_42600")));
