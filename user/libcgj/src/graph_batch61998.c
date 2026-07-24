/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch61998: milestone 62000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_62000(void);
 *     - Returns the milestone tag for the milestone 62000 continuum
 *       (always 62000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_62000  (alias)
 *   __libcgj_batch61998_marker = "libcgj-batch61998"
 *
 * Milestone 62000 exclusive continuum CREATE-ONLY (25891-62000). Unique
 * gj_milestone_tag_62000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_62000 (batch62000), and gj_continuum_wave_62000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–62000): host soft probes
 * (cgj_soft_milestone_62000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=62000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 61998. */
const char __libcgj_batch61998_marker[] = "libcgj-batch61998";

/* Soft milestone tag for wave 62000 (product identity = 62000). */
#define B61998_MILESTONE_TAG  62000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b61998_tag(void)
{
	return B61998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_62000 - report soft milestone product tag for wave 62000.
 *
 * Always returns 62000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 62000.
 */
uint32_t
gj_milestone_tag_62000(void)
{
	(void)NULL;
	return b61998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_62000(void)
    __attribute__((alias("gj_milestone_tag_62000")));
