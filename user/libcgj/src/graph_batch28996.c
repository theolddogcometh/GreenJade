/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28996: smoke soft lamp (wave 29000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_29000(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 29000
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_29000  (alias)
 *   __libcgj_batch28996_marker = "libcgj-batch28996"
 *
 * Milestone 29000 exclusive continuum CREATE-ONLY (28991-29000). Unique
 * gj_smoke_soft_29000 surface only; no multi-def. Distinct from
 * gj_smoke_soft_28900 / gj_smoke_soft_28800 / gj_smoke_soft_28700 and
 * sibling 29000 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28991–29000): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_29000). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28996. */
const char __libcgj_batch28996_marker[] = "libcgj-batch28996";

/* Smoke soft ready lamp for wave 29000 (static soft surface present). */
#define B28996_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28996_soft(void)
{
	return B28996_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_29000 - report smoke soft lamp for wave 29000.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_29000(void)
{
	(void)NULL;
	return b28996_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_29000(void)
    __attribute__((alias("gj_smoke_soft_29000")));
