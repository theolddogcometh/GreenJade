/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38898: milestone 38900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_38900(void);
 *     - Returns the milestone tag for the milestone 38900 continuum
 *       (always 38900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_38900  (alias)
 *   __libcgj_batch38898_marker = "libcgj-batch38898"
 *
 * Milestone 38900 exclusive continuum CREATE-ONLY (38891-38900). Unique
 * gj_milestone_tag_38900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_38800 / gj_milestone_tag_38700 / gj_milestone_tag_38600,
 * gj_graph_milestone_38900 (batch38900), and gj_continuum_wave_38900
 * (batch38899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (38891–38900): host soft probes
 * (cgj_soft_milestone_38900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=38900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 38898. */
const char __libcgj_batch38898_marker[] = "libcgj-batch38898";

/* Soft milestone tag for wave 38900 (product identity = 38900). */
#define B38898_MILESTONE_TAG  38900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38898_tag(void)
{
	return B38898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_38900 - report soft milestone product tag for wave 38900.
 *
 * Always returns 38900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 38900.
 */
uint32_t
gj_milestone_tag_38900(void)
{
	(void)NULL;
	return b38898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_38900(void)
    __attribute__((alias("gj_milestone_tag_38900")));
