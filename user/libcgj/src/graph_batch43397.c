/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43397: dyn soft lamp (wave 43400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dyn_soft_43400(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 43400
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_dyn_soft_43400  (alias)
 *   __libcgj_batch43397_marker = "libcgj-batch43397"
 *
 * Milestone 43400 exclusive continuum CREATE-ONLY (43391-43400). Unique
 * gj_dyn_soft_43400 surface only; no multi-def. Distinct from
 * gj_dyn_soft_43300 / gj_dyn_soft_43200 / gj_dyn_soft_43100 and
 * sibling 43400 milestone symbols (smoke_soft, continuum_ready, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (43391–43400): pairs with dyn/dlopen soft hosts
 * (cgj_dyn_smoke / cgj_soft_milestone_43400). Soft rule: missing symbol
 * → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=43400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 43397. */
const char __libcgj_batch43397_marker[] = "libcgj-batch43397";

/* Dyn soft ready lamp for wave 43400 (dyn soft surface present). */
#define B43397_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43397_soft(void)
{
	return B43397_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_soft_43400 - report dyn soft lamp for wave 43400.
 *
 * Always returns 1u (dyn soft surface ready). Soft pure-data product
 * tag; does not dlopen product SO or probe version maps. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_dyn_soft_43400(void)
{
	(void)NULL;
	return b43397_soft();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_dyn_soft_43400(void)
    __attribute__((alias("gj_dyn_soft_43400")));
