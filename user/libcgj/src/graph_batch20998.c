/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20998: milestone 21000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_21000(void);
 *     - Returns the milestone tag for the milestone 21000 continuum
 *       (always 21000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_21000  (alias)
 *   __libcgj_batch20998_marker = "libcgj-batch20998"
 *
 * Milestone 21000 exclusive continuum CREATE-ONLY (20991-21000). Unique
 * gj_milestone_tag_21000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_20900 / gj_milestone_tag_20800 / gj_milestone_tag_20700,
 * gj_graph_milestone_21000 (batch21000), and gj_continuum_wave_21000
 * (batch20999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (20991–21000): host soft probes
 * (cgj_soft_milestone_21000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20998. */
const char __libcgj_batch20998_marker[] = "libcgj-batch20998";

/* Soft milestone tag for wave 21000 (product identity = 21000). */
#define B20998_MILESTONE_TAG  21000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20998_tag(void)
{
	return B20998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_21000 - report soft milestone product tag for wave 21000.
 *
 * Always returns 21000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21000.
 */
uint32_t
gj_milestone_tag_21000(void)
{
	(void)NULL;
	return b20998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_21000(void)
    __attribute__((alias("gj_milestone_tag_21000")));
