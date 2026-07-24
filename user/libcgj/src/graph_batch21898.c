/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21898: milestone 21900 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_21900(void);
 *     - Returns the milestone tag for the milestone 21900 continuum
 *       (always 21900u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_21900  (alias)
 *   __libcgj_batch21898_marker = "libcgj-batch21898"
 *
 * Milestone 21900 exclusive continuum CREATE-ONLY (21891-21900). Unique
 * gj_milestone_tag_21900 surface only; no multi-def. Distinct from
 * gj_milestone_tag_21800 / gj_milestone_tag_21700 / gj_milestone_tag_21600,
 * gj_graph_milestone_21900 (batch21900), and gj_continuum_wave_21900
 * (batch21899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21891–21900): host soft probes
 * (cgj_soft_milestone_21900.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21900 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21898. */
const char __libcgj_batch21898_marker[] = "libcgj-batch21898";

/* Soft milestone tag for wave 21900 (product identity = 21900). */
#define B21898_MILESTONE_TAG  21900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21898_tag(void)
{
	return B21898_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_21900 - report soft milestone product tag for wave 21900.
 *
 * Always returns 21900u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21900.
 */
uint32_t
gj_milestone_tag_21900(void)
{
	(void)NULL;
	return b21898_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_21900(void)
    __attribute__((alias("gj_milestone_tag_21900")));
