/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36898: milestone 36900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_36900(void);
 *     - Returns the milestone tag for the milestone 36900 continuum
 *       (always 36900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_36900  (alias)
 *   __libcgj_batch36898_marker = "libcgj-batch36898"
 *
 * Milestone 36900 exclusive continuum CREATE-ONLY (36891-36900). Unique
 * gj_milestone_tag_36900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36800 / gj_milestone_tag_36700 / gj_milestone_tag_36600,
 * gj_graph_milestone_36900 (batch36900), and gj_continuum_wave_36900
 * (batch36899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36891–36900): host soft probes
 * (cgj_soft_milestone_36900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=36900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36898. */
const char __libcgj_batch36898_marker[] = "libcgj-batch36898";

/* Soft milestone tag for wave 36900 (product identity = 36900). */
#define B36898_MILESTONE_TAG  36900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36898_tag(void)
{
	return B36898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_36900 - report soft milestone product tag for wave 36900.
 *
 * Always returns 36900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 36900.
 */
uint32_t
gj_milestone_tag_36900(void)
{
	(void)NULL;
	return b36898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_36900(void)
    __attribute__((alias("gj_milestone_tag_36900")));
