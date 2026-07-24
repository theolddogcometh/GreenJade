/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87198: milestone 87200 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_87200(void);
 *     - Returns the milestone tag for the milestone 87200 continuum
 *       (always 87200u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_87200  (alias)
 *   __libcgj_batch87198_marker = "libcgj-batch87198"
 *
 * Milestone 87200 exclusive continuum CREATE-ONLY (25891-87200). Unique
 * gj_milestone_tag_87200 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_87200 (batch87200), and gj_continuum_wave_87200
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–87200): host soft probes
 * (cgj_soft_milestone_87200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=87200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87198. */
const char __libcgj_batch87198_marker[] = "libcgj-batch87198";

/* Soft milestone tag for wave 87200 (product identity = 87200). */
#define B87198_MILESTONE_TAG  87200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87198_tag(void)
{
	return B87198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_87200 - report soft milestone product tag for wave 87200.
 *
 * Always returns 87200u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 87200.
 */
uint32_t
gj_milestone_tag_87200(void)
{
	(void)NULL;
	return b87198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_87200(void)
    __attribute__((alias("gj_milestone_tag_87200")));
