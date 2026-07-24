/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29498: milestone 29500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_29500(void);
 *     - Returns the milestone tag for the milestone 29500 continuum
 *       (always 29500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_29500  (alias)
 *   __libcgj_batch29498_marker = "libcgj-batch29498"
 *
 * Milestone 29500 exclusive continuum CREATE-ONLY (29491-29500). Unique
 * gj_milestone_tag_29500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_29000 / gj_milestone_tag_28900 / gj_milestone_tag_28800,
 * gj_graph_milestone_29500 (batch29500), and gj_continuum_wave_29500
 * (batch29499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (29491–29500): host soft probes
 * (cgj_soft_milestone_29500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=29500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29498. */
const char __libcgj_batch29498_marker[] = "libcgj-batch29498";

/* Soft milestone tag for wave 29500 (product identity = 29500). */
#define B29498_MILESTONE_TAG  29500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29498_tag(void)
{
	return B29498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_29500 - report soft milestone product tag for wave 29500.
 *
 * Always returns 29500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 29500.
 */
uint32_t
gj_milestone_tag_29500(void)
{
	(void)NULL;
	return b29498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_29500(void)
    __attribute__((alias("gj_milestone_tag_29500")));
