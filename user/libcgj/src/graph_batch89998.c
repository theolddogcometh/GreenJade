/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch89998: milestone 90000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_90000(void);
 *     - Returns the milestone tag for the milestone 90000 continuum
 *       (always 90000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_90000  (alias)
 *   __libcgj_batch89998_marker = "libcgj-batch89998"
 *
 * Milestone 90000 exclusive continuum CREATE-ONLY (25891-90000). Unique
 * gj_milestone_tag_90000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_90000 (batch90000), and gj_continuum_wave_90000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–90000): host soft probes
 * (cgj_soft_milestone_90000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=90000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 89998. */
const char __libcgj_batch89998_marker[] = "libcgj-batch89998";

/* Soft milestone tag for wave 90000 (product identity = 90000). */
#define B89998_MILESTONE_TAG  90000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b89998_tag(void)
{
	return B89998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_90000 - report soft milestone product tag for wave 90000.
 *
 * Always returns 90000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 90000.
 */
uint32_t
gj_milestone_tag_90000(void)
{
	(void)NULL;
	return b89998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_90000(void)
    __attribute__((alias("gj_milestone_tag_90000")));
