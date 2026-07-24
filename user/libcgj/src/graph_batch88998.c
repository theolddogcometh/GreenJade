/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch88998: milestone 89000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_89000(void);
 *     - Returns the milestone tag for the milestone 89000 continuum
 *       (always 89000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_89000  (alias)
 *   __libcgj_batch88998_marker = "libcgj-batch88998"
 *
 * Milestone 89000 exclusive continuum CREATE-ONLY (25891-89000). Unique
 * gj_milestone_tag_89000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_89000 (batch89000), and gj_continuum_wave_89000
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–89000): host soft probes
 * (cgj_soft_milestone_89000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=89000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 88998. */
const char __libcgj_batch88998_marker[] = "libcgj-batch88998";

/* Soft milestone tag for wave 89000 (product identity = 89000). */
#define B88998_MILESTONE_TAG  89000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b88998_tag(void)
{
	return B88998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_89000 - report soft milestone product tag for wave 89000.
 *
 * Always returns 89000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 89000.
 */
uint32_t
gj_milestone_tag_89000(void)
{
	(void)NULL;
	return b88998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_89000(void)
    __attribute__((alias("gj_milestone_tag_89000")));
