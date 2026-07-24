/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48998: milestone 49000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_49000(void);
 *     - Returns the milestone tag for the milestone 49000 continuum
 *       (always 49000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_49000  (alias)
 *   __libcgj_batch48998_marker = "libcgj-batch48998"
 *
 * Milestone 49000 exclusive continuum CREATE-ONLY (48991-49000). Unique
 * gj_milestone_tag_49000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_48900 / gj_milestone_tag_48800 / gj_milestone_tag_48700,
 * gj_graph_milestone_49000 (batch49000), and gj_continuum_wave_49000
 * (batch48999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (48991–49000): host soft probes
 * (cgj_soft_milestone_49000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=49000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48998. */
const char __libcgj_batch48998_marker[] = "libcgj-batch48998";

/* Soft milestone tag for wave 49000 (product identity = 49000). */
#define B48998_MILESTONE_TAG  49000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48998_tag(void)
{
	return B48998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_49000 - report soft milestone product tag for wave 49000.
 *
 * Always returns 49000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 49000.
 */
uint32_t
gj_milestone_tag_49000(void)
{
	(void)NULL;
	return b48998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_49000(void)
    __attribute__((alias("gj_milestone_tag_49000")));
