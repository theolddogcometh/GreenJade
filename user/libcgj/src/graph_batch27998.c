/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27998: milestone 28000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28000(void);
 *     - Returns the milestone tag for the milestone 28000 continuum
 *       (always 28000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28000  (alias)
 *   __libcgj_batch27998_marker = "libcgj-batch27998"
 *
 * Milestone 28000 exclusive continuum CREATE-ONLY (27991-28000). Unique
 * gj_milestone_tag_28000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_27900 / gj_milestone_tag_27800 / gj_milestone_tag_27700,
 * gj_graph_milestone_28000 (batch28000), and gj_continuum_wave_28000
 * (batch27999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27991–28000): host soft probes
 * (cgj_soft_milestone_28000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27998. */
const char __libcgj_batch27998_marker[] = "libcgj-batch27998";

/* Soft milestone tag for wave 28000 (product identity = 28000). */
#define B27998_MILESTONE_TAG  28000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27998_tag(void)
{
	return B27998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28000 - report soft milestone product tag for wave 28000.
 *
 * Always returns 28000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28000.
 */
uint32_t
gj_milestone_tag_28000(void)
{
	(void)NULL;
	return b27998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28000(void)
    __attribute__((alias("gj_milestone_tag_28000")));
