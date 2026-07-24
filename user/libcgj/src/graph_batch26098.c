/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26098: milestone 26100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_26100(void);
 *     - Returns the milestone tag for the milestone 26100 continuum
 *       (always 26100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_26100  (alias)
 *   __libcgj_batch26098_marker = "libcgj-batch26098"
 *
 * Milestone 26100 exclusive continuum CREATE-ONLY (26091-26100). Unique
 * gj_milestone_tag_26100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_26000 / gj_milestone_tag_25900 / gj_milestone_tag_25800,
 * gj_graph_milestone_26100 (batch26100), and gj_continuum_wave_26100
 * (batch26099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (26091–26100): host soft probes
 * (cgj_soft_milestone_26100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=26100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26098. */
const char __libcgj_batch26098_marker[] = "libcgj-batch26098";

/* Soft milestone tag for wave 26100 (product identity = 26100). */
#define B26098_MILESTONE_TAG  26100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26098_tag(void)
{
	return B26098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_26100 - report soft milestone product tag for wave 26100.
 *
 * Always returns 26100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 26100.
 */
uint32_t
gj_milestone_tag_26100(void)
{
	(void)NULL;
	return b26098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_26100(void)
    __attribute__((alias("gj_milestone_tag_26100")));
