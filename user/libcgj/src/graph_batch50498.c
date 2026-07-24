/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50498: milestone 50500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_50500(void);
 *     - Returns the milestone tag for the milestone 50500 continuum
 *       (always 50500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_50500  (alias)
 *   __libcgj_batch50498_marker = "libcgj-batch50498"
 *
 * Milestone 50500 exclusive continuum CREATE-ONLY (50491-50500). Unique
 * gj_milestone_tag_50500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_50400 / gj_milestone_tag_50300 / gj_milestone_tag_50200,
 * gj_graph_milestone_50500 (batch50500), and gj_continuum_wave_50500
 * (batch50499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (50491–50500): host soft probes
 * (cgj_soft_milestone_50500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=50500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50498. */
const char __libcgj_batch50498_marker[] = "libcgj-batch50498";

/* Soft milestone tag for wave 50500 (product identity = 50500). */
#define B50498_MILESTONE_TAG  50500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50498_tag(void)
{
	return B50498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_50500 - report soft milestone product tag for wave 50500.
 *
 * Always returns 50500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 50500.
 */
uint32_t
gj_milestone_tag_50500(void)
{
	(void)NULL;
	return b50498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_50500(void)
    __attribute__((alias("gj_milestone_tag_50500")));
