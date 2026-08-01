/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95798: milestone 95800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_95800(void);
 *     - Returns the milestone tag for the milestone 95800 continuum
 *       (always 95800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_95800  (alias)
 *   __libcgj_batch95798_marker = "libcgj-batch95798"
 *
 * Milestone 95800 exclusive continuum CREATE-ONLY (25891-95800). Unique
 * gj_milestone_tag_95800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_95800 (batch95800), and gj_continuum_wave_95800
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–95800): host soft probes
 * (cgj_soft_milestone_95800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=95800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95798. */
const char __libcgj_batch95798_marker[] = "libcgj-batch95798";

/* Soft milestone tag for wave 95800 (product identity = 95800). */
#define B95798_MILESTONE_TAG  95800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95798_tag(void)
{
	return B95798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_95800 - report soft milestone product tag for wave 95800.
 *
 * Always returns 95800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 95800.
 */
uint32_t
gj_milestone_tag_95800(void)
{
	(void)NULL;
	return b95798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_95800(void)
    __attribute__((alias("gj_milestone_tag_95800")));
