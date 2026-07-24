/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25998: milestone 26000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26000(void);
 *     - Returns the milestone tag for the milestone 26000 continuum
 *       (always 26000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26000  (alias)
 *   __libcgj_batch25998_marker = "libcgj-batch25998"
 *
 * Milestone 26000 exclusive continuum CREATE-ONLY (25991-26000). Unique
 * gj_milestone_tag_26000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25900 / gj_milestone_tag_25800 / gj_milestone_tag_25700,
 * gj_graph_milestone_26000 (batch26000), and gj_continuum_wave_26000
 * (batch25999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25991–26000): host soft probes
 * (cgj_soft_milestone_26000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25998. */
const char __libcgj_batch25998_marker[] = "libcgj-batch25998";

/* Soft milestone tag for wave 26000 (product identity = 26000). */
#define B25998_MILESTONE_TAG  26000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25998_tag(void)
{
	return B25998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26000 - report soft milestone product tag for wave 26000.
 *
 * Always returns 26000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26000.
 */
uint32_t
gj_milestone_tag_26000(void)
{
	(void)NULL;
	return b25998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26000(void)
    __attribute__((alias("gj_milestone_tag_26000")));
