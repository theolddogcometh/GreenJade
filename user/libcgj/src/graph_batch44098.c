/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44098: milestone 44100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_44100(void);
 *     - Returns the milestone tag for the milestone 44100 continuum
 *       (always 44100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_44100  (alias)
 *   __libcgj_batch44098_marker = "libcgj-batch44098"
 *
 * Milestone 44100 exclusive continuum CREATE-ONLY (44091-44100). Unique
 * gj_milestone_tag_44100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_44000 / gj_milestone_tag_43900 / gj_milestone_tag_43800,
 * gj_graph_milestone_44100 (batch44100), and gj_continuum_wave_44100
 * (batch44099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44091–44100): host soft probes
 * (cgj_soft_milestone_44100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44098. */
const char __libcgj_batch44098_marker[] = "libcgj-batch44098";

/* Soft milestone tag for wave 44100 (product identity = 44100). */
#define B44098_MILESTONE_TAG  44100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44098_tag(void)
{
	return B44098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_44100 - report soft milestone product tag for wave 44100.
 *
 * Always returns 44100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 44100.
 */
uint32_t
gj_milestone_tag_44100(void)
{
	(void)NULL;
	return b44098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_44100(void)
    __attribute__((alias("gj_milestone_tag_44100")));
