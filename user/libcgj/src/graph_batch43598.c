/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch43598: milestone 43600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_43600(void);
 *     - Returns the milestone tag for the milestone 43600 continuum
 *       (always 43600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_43600  (alias)
 *   __libcgj_batch43598_marker = "libcgj-batch43598"
 *
 * Milestone 43600 exclusive continuum CREATE-ONLY (43591-43600). Unique
 * gj_milestone_tag_43600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_43500 / gj_milestone_tag_43400 / gj_milestone_tag_43300,
 * gj_graph_milestone_43600 (batch43600), and gj_continuum_wave_43600
 * (batch43599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (43591–43600): host soft probes
 * (cgj_soft_milestone_43600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=43600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 43598. */
const char __libcgj_batch43598_marker[] = "libcgj-batch43598";

/* Soft milestone tag for wave 43600 (product identity = 43600). */
#define B43598_MILESTONE_TAG  43600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b43598_tag(void)
{
	return B43598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_43600 - report soft milestone product tag for wave 43600.
 *
 * Always returns 43600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 43600.
 */
uint32_t
gj_milestone_tag_43600(void)
{
	(void)NULL;
	return b43598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_43600(void)
    __attribute__((alias("gj_milestone_tag_43600")));
