/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80798: milestone 80800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_80800(void);
 *     - Returns the milestone tag for the milestone 80800 continuum
 *       (always 80800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_80800  (alias)
 *   __libcgj_batch80798_marker = "libcgj-batch80798"
 *
 * Milestone 80800 exclusive continuum CREATE-ONLY (25891-80800). Unique
 * gj_milestone_tag_80800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_80800 (batch80800), and gj_continuum_wave_80800
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–80800): host soft probes
 * (cgj_soft_milestone_80800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=80800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 80798. */
const char __libcgj_batch80798_marker[] = "libcgj-batch80798";

/* Soft milestone tag for wave 80800 (product identity = 80800). */
#define B80798_MILESTONE_TAG  80800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b80798_tag(void)
{
	return B80798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_80800 - report soft milestone product tag for wave 80800.
 *
 * Always returns 80800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 80800.
 */
uint32_t
gj_milestone_tag_80800(void)
{
	(void)NULL;
	return b80798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_80800(void)
    __attribute__((alias("gj_milestone_tag_80800")));
