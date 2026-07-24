/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20498: milestone 20500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_20500(void);
 *     - Returns the milestone tag for the milestone 20500 continuum
 *       (always 20500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_20500  (alias)
 *   __libcgj_batch20498_marker = "libcgj-batch20498"
 *
 * Milestone 20500 exclusive continuum CREATE-ONLY (20491-20500). Unique
 * gj_milestone_tag_20500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_20400 / gj_milestone_tag_20300 / gj_milestone_tag_20200,
 * gj_graph_milestone_20500 (batch20500), and gj_continuum_wave_20500
 * (batch20499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20491–20500): host soft probes
 * (cgj_soft_milestone_20500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=20500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20498. */
const char __libcgj_batch20498_marker[] = "libcgj-batch20498";

/* Soft milestone tag for wave 20500 (product identity = 20500). */
#define B20498_MILESTONE_TAG  20500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20498_tag(void)
{
	return B20498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_20500 - report soft milestone product tag for wave 20500.
 *
 * Always returns 20500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 20500.
 */
uint32_t
gj_milestone_tag_20500(void)
{
	(void)NULL;
	return b20498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_20500(void)
    __attribute__((alias("gj_milestone_tag_20500")));
