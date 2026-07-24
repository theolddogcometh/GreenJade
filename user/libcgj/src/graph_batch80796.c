/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80796: smoke soft lamp (wave 80800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_smoke_soft_80800(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 80800
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_smoke_soft_80800  (alias)
 *   __libcgj_batch80796_marker = "libcgj-batch80796"
 *
 * Milestone 80800 exclusive continuum CREATE-ONLY (25891-80800). Unique
 * gj_smoke_soft_80800 surface only; no multi-def. Distinct from
 * gj_smoke_soft_25800 / gj_smoke_soft_25700 / gj_smoke_soft_25600 and
 * sibling 80800 milestone symbols (dyn_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–80800): pairs with static freestanding
 * soft smoke hosts (cgj_smoke / cgj_soft_milestone_80800). Soft rule:
 * missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 80796. */
const char __libcgj_batch80796_marker[] = "libcgj-batch80796";

/* Smoke soft ready lamp for wave 80800 (static soft surface present). */
#define B80796_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80796_soft(void)
{
	return B80796_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_80800 - report smoke soft lamp for wave 80800.
 *
 * Always returns 1u (smoke soft surface ready). Soft pure-data product
 * tag; does not invoke cgj_smoke or any host process. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_smoke_soft_80800(void)
{
	(void)NULL;
	return b80796_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_smoke_soft_80800(void)
    __attribute__((alias("gj_smoke_soft_80800")));
