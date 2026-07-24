/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch45996: smoke soft lamp (wave 46000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_46000(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 46000
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_46000  (alias)
 *   __libcgj_batch45996_marker = "libcgj-batch45996"
 *
 * Milestone 46000 exclusive continuum CREATE-ONLY (45991-46000). Unique
 * gj_smoke_soft_46000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_45900 / gj_smoke_soft_45800 / gj_smoke_soft_45700 and
 * sibling 46000 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (45991–46000): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_46000). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=46000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 45996. */
const char __libcgj_batch45996_marker[] = "libcgj-batch45996";

/* Smoke soft ready lamp for wave 46000 (static soft surface present). */
#define B45996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b45996_soft(void)
{
	return B45996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_46000 - report smoke soft lamp for wave 46000.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_46000(void)
{
	(void)NULL;
	return b45996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_46000(void)
    __attribute__((alias("gj_smoke_soft_46000")));
