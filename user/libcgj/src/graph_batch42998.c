/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42998: milestone 43000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_43000(void);
 *     - Returns the milestone tag for the milestone 43000 continuum
 *       (always 43000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_43000  (alias)
 *   __libcgj_batch42998_marker = "libcgj-batch42998"
 *
 * Milestone 43000 exclusive continuum CREATE-ONLY (42991-43000). Unique
 * gj_milestone_tag_43000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_42900 / gj_milestone_tag_42800 / gj_milestone_tag_42700,
 * gj_graph_milestone_43000 (batch43000), and gj_continuum_wave_43000
 * (batch42999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (42991–43000): host soft probes
 * (cgj_soft_milestone_43000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=43000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42998. */
const char __libcgj_batch42998_marker[] = "libcgj-batch42998";

/* Soft milestone tag for wave 43000 (product identity = 43000). */
#define B42998_MILESTONE_TAG  43000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42998_tag(void)
{
	return B42998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_43000 - report soft milestone product tag for wave 43000.
 *
 * Always returns 43000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 43000.
 */
uint32_t
gj_milestone_tag_43000(void)
{
	(void)NULL;
	return b42998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_43000(void)
    __attribute__((alias("gj_milestone_tag_43000")));
