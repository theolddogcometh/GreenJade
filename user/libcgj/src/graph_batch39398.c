/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39398: milestone 39400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_39400(void);
 *     - Returns the milestone tag for the milestone 39400 continuum
 *       (always 39400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_39400  (alias)
 *   __libcgj_batch39398_marker = "libcgj-batch39398"
 *
 * Milestone 39400 exclusive continuum CREATE-ONLY (39391-39400). Unique
 * gj_milestone_tag_39400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_39300 / gj_milestone_tag_39200 / gj_milestone_tag_39100,
 * gj_graph_milestone_39400 (batch39400), and gj_continuum_wave_39400
 * (batch39399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (39391–39400): host soft probes
 * (cgj_soft_milestone_39400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=39400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 39398. */
const char __libcgj_batch39398_marker[] = "libcgj-batch39398";

/* Soft milestone tag for wave 39400 (product identity = 39400). */
#define B39398_MILESTONE_TAG  39400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39398_tag(void)
{
	return B39398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_39400 - report soft milestone product tag for wave 39400.
 *
 * Always returns 39400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 39400.
 */
uint32_t
gj_milestone_tag_39400(void)
{
	(void)NULL;
	return b39398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_39400(void)
    __attribute__((alias("gj_milestone_tag_39400")));
