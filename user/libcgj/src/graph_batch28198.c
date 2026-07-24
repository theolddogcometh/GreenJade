/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28198: milestone 28200 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_28200(void);
 *     - Returns the milestone tag for the milestone 28200 continuum
 *       (always 28200u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_28200  (alias)
 *   __libcgj_batch28198_marker = "libcgj-batch28198"
 *
 * Milestone 28200 exclusive continuum CREATE-ONLY (28191-28200). Unique
 * gj_milestone_tag_28200 surface only; no multi-def. Distinct from
 * gj_milestone_tag_28100 / gj_milestone_tag_28000 / gj_milestone_tag_27900,
 * gj_graph_milestone_28200 (batch28200), and gj_continuum_wave_28200
 * (batch28199).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (28191–28200): host soft probes
 * (cgj_soft_milestone_28200.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=28200 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28198. */
const char __libcgj_batch28198_marker[] = "libcgj-batch28198";

/* Soft milestone tag for wave 28200 (product identity = 28200). */
#define B28198_MILESTONE_TAG  28200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28198_tag(void)
{
	return B28198_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_28200 - report soft milestone product tag for wave 28200.
 *
 * Always returns 28200u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 28200.
 */
uint32_t
gj_milestone_tag_28200(void)
{
	(void)NULL;
	return b28198_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_28200(void)
    __attribute__((alias("gj_milestone_tag_28200")));
