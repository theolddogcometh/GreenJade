/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42498: milestone 42500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_42500(void);
 *     - Returns the milestone tag for the milestone 42500 continuum
 *       (always 42500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_42500  (alias)
 *   __libcgj_batch42498_marker = "libcgj-batch42498"
 *
 * Milestone 42500 exclusive continuum CREATE-ONLY (42491-42500). Unique
 * gj_milestone_tag_42500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_42400 / gj_milestone_tag_42300 / gj_milestone_tag_42200,
 * gj_graph_milestone_42500 (batch42500), and gj_continuum_wave_42500
 * (batch42499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42491–42500): host soft probes
 * (cgj_soft_milestone_42500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=42500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42498. */
const char __libcgj_batch42498_marker[] = "libcgj-batch42498";

/* Soft milestone tag for wave 42500 (product identity = 42500). */
#define B42498_MILESTONE_TAG  42500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42498_tag(void)
{
	return B42498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_42500 - report soft milestone product tag for wave 42500.
 *
 * Always returns 42500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 42500.
 */
uint32_t
gj_milestone_tag_42500(void)
{
	(void)NULL;
	return b42498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_42500(void)
    __attribute__((alias("gj_milestone_tag_42500")));
