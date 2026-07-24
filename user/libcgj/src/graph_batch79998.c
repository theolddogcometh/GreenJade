/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch79998: milestone 80000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_80000(void);
 *     - Returns the milestone tag for the milestone 80000 continuum
 *       (always 80000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_80000  (alias)
 *   __libcgj_batch79998_marker = "libcgj-batch79998"
 *
 * Milestone 80000 exclusive continuum CREATE-ONLY (25891-80000). Unique
 * gj_milestone_tag_80000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_80000 (batch80000), and gj_continuum_wave_80000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–80000): host soft probes
 * (cgj_soft_milestone_80000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 79998. */
const char __libcgj_batch79998_marker[] = "libcgj-batch79998";

/* Soft milestone tag for wave 80000 (product identity = 80000). */
#define B79998_MILESTONE_TAG  80000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b79998_tag(void)
{
	return B79998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_80000 - report soft milestone product tag for wave 80000.
 *
 * Always returns 80000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 80000.
 */
uint32_t
gj_milestone_tag_80000(void)
{
	(void)NULL;
	return b79998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_80000(void)
    __attribute__((alias("gj_milestone_tag_80000")));
