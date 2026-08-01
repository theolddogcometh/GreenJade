/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95498: milestone 95500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_95500(void);
 *     - Returns the milestone tag for the milestone 95500 continuum
 *       (always 95500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_95500  (alias)
 *   __libcgj_batch95498_marker = "libcgj-batch95498"
 *
 * Milestone 95500 exclusive continuum CREATE-ONLY (25891-95500). Unique
 * gj_milestone_tag_95500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_95500 (batch95500), and gj_continuum_wave_95500
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–95500): host soft probes
 * (cgj_soft_milestone_95500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=95500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95498. */
const char __libcgj_batch95498_marker[] = "libcgj-batch95498";

/* Soft milestone tag for wave 95500 (product identity = 95500). */
#define B95498_MILESTONE_TAG  95500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95498_tag(void)
{
	return B95498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_95500 - report soft milestone product tag for wave 95500.
 *
 * Always returns 95500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 95500.
 */
uint32_t
gj_milestone_tag_95500(void)
{
	(void)NULL;
	return b95498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_95500(void)
    __attribute__((alias("gj_milestone_tag_95500")));
