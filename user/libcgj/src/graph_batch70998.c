/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch70998: milestone 71000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_71000(void);
 *     - Returns the milestone tag for the milestone 71000 continuum
 *       (always 71000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_71000  (alias)
 *   __libcgj_batch70998_marker = "libcgj-batch70998"
 *
 * Milestone 71000 exclusive continuum CREATE-ONLY (25891-71000). Unique
 * gj_milestone_tag_71000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_71000 (batch71000), and gj_continuum_wave_71000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–71000): host soft probes
 * (cgj_soft_milestone_71000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=71000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 70998. */
const char __libcgj_batch70998_marker[] = "libcgj-batch70998";

/* Soft milestone tag for wave 71000 (product identity = 71000). */
#define B70998_MILESTONE_TAG  71000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b70998_tag(void)
{
	return B70998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_71000 - report soft milestone product tag for wave 71000.
 *
 * Always returns 71000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 71000.
 */
uint32_t
gj_milestone_tag_71000(void)
{
	(void)NULL;
	return b70998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_71000(void)
    __attribute__((alias("gj_milestone_tag_71000")));
