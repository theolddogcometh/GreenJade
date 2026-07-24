/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22998: milestone 23000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_23000(void);
 *     - Returns the milestone tag for the milestone 23000 continuum
 *       (always 23000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_23000  (alias)
 *   __libcgj_batch22998_marker = "libcgj-batch22998"
 *
 * Milestone 23000 exclusive continuum CREATE-ONLY (22991-23000). Unique
 * gj_milestone_tag_23000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_22900 / gj_milestone_tag_22800 / gj_milestone_tag_22700,
 * gj_graph_milestone_23000 (batch23000), and gj_continuum_wave_23000
 * (batch22999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (22991–23000): host soft probes
 * (cgj_soft_milestone_23000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=23000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22998. */
const char __libcgj_batch22998_marker[] = "libcgj-batch22998";

/* Soft milestone tag for wave 23000 (product identity = 23000). */
#define B22998_MILESTONE_TAG  23000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22998_tag(void)
{
	return B22998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_23000 - report soft milestone product tag for wave 23000.
 *
 * Always returns 23000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 23000.
 */
uint32_t
gj_milestone_tag_23000(void)
{
	(void)NULL;
	return b22998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_23000(void)
    __attribute__((alias("gj_milestone_tag_23000")));
