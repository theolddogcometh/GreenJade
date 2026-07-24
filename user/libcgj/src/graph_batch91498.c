/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch91498: milestone 91500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_91500(void);
 *     - Returns the milestone tag for the milestone 91500 continuum
 *       (always 91500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_91500  (alias)
 *   __libcgj_batch91498_marker = "libcgj-batch91498"
 *
 * Milestone 91500 exclusive continuum CREATE-ONLY (25891-91500). Unique
 * gj_milestone_tag_91500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_91500 (batch91500), and gj_continuum_wave_91500
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–91500): host soft probes
 * (cgj_soft_milestone_91500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=91500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 91498. */
const char __libcgj_batch91498_marker[] = "libcgj-batch91498";

/* Soft milestone tag for wave 91500 (product identity = 91500). */
#define B91498_MILESTONE_TAG  91500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b91498_tag(void)
{
	return B91498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_91500 - report soft milestone product tag for wave 91500.
 *
 * Always returns 91500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 91500.
 */
uint32_t
gj_milestone_tag_91500(void)
{
	(void)NULL;
	return b91498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_91500(void)
    __attribute__((alias("gj_milestone_tag_91500")));
