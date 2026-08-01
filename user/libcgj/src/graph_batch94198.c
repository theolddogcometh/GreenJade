/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94198: milestone 94200 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_94200(void);
 *     - Returns the milestone tag for the milestone 94200 continuum
 *       (always 94200u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_94200  (alias)
 *   __libcgj_batch94198_marker = "libcgj-batch94198"
 *
 * Milestone 94200 exclusive continuum CREATE-ONLY (25891-94200). Unique
 * gj_milestone_tag_94200 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_94200 (batch94200), and gj_continuum_wave_94200
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–94200): host soft probes
 * (cgj_soft_milestone_94200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=94200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94198. */
const char __libcgj_batch94198_marker[] = "libcgj-batch94198";

/* Soft milestone tag for wave 94200 (product identity = 94200). */
#define B94198_MILESTONE_TAG  94200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94198_tag(void)
{
	return B94198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_94200 - report soft milestone product tag for wave 94200.
 *
 * Always returns 94200u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 94200.
 */
uint32_t
gj_milestone_tag_94200(void)
{
	(void)NULL;
	return b94198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_94200(void)
    __attribute__((alias("gj_milestone_tag_94200")));
