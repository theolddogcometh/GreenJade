/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32498: milestone 32500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_32500(void);
 *     - Returns the milestone tag for the milestone 32500 continuum
 *       (always 32500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_32500  (alias)
 *   __libcgj_batch32498_marker = "libcgj-batch32498"
 *
 * Milestone 32500 exclusive continuum CREATE-ONLY (32491-32500). Unique
 * gj_milestone_tag_32500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_32400 / gj_milestone_tag_32300 / gj_milestone_tag_32200,
 * gj_graph_milestone_32500 (batch32500), and gj_continuum_wave_32500
 * (batch32499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (32491–32500): host soft probes
 * (cgj_soft_milestone_32500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=32500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 32498. */
const char __libcgj_batch32498_marker[] = "libcgj-batch32498";

/* Soft milestone tag for wave 32500 (product identity = 32500). */
#define B32498_MILESTONE_TAG  32500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b32498_tag(void)
{
	return B32498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_32500 - report soft milestone product tag for wave 32500.
 *
 * Always returns 32500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 32500.
 */
uint32_t
gj_milestone_tag_32500(void)
{
	(void)NULL;
	return b32498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_32500(void)
    __attribute__((alias("gj_milestone_tag_32500")));
