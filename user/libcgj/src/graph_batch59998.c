/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch59998: milestone 60000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_60000(void);
 *     - Returns the milestone tag for the milestone 60000 continuum
 *       (always 60000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_60000  (alias)
 *   __libcgj_batch59998_marker = "libcgj-batch59998"
 *
 * Milestone 60000 exclusive continuum CREATE-ONLY (25891-60000). Unique
 * gj_milestone_tag_60000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_60000 (batch60000), and gj_continuum_wave_60000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–60000): host soft probes
 * (cgj_soft_milestone_60000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=60000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 59998. */
const char __libcgj_batch59998_marker[] = "libcgj-batch59998";

/* Soft milestone tag for wave 60000 (product identity = 60000). */
#define B59998_MILESTONE_TAG  60000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b59998_tag(void)
{
	return B59998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_60000 - report soft milestone product tag for wave 60000.
 *
 * Always returns 60000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 60000.
 */
uint32_t
gj_milestone_tag_60000(void)
{
	(void)NULL;
	return b59998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_60000(void)
    __attribute__((alias("gj_milestone_tag_60000")));
