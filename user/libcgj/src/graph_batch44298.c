/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44298: milestone 44300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_44300(void);
 *     - Returns the milestone tag for the milestone 44300 continuum
 *       (always 44300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_44300  (alias)
 *   __libcgj_batch44298_marker = "libcgj-batch44298"
 *
 * Milestone 44300 exclusive continuum CREATE-ONLY (44291-44300). Unique
 * gj_milestone_tag_44300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_44200 / gj_milestone_tag_44100 / gj_milestone_tag_44000,
 * gj_graph_milestone_44300 (batch44300), and gj_continuum_wave_44300
 * (batch44299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44291–44300): host soft probes
 * (cgj_soft_milestone_44300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44298. */
const char __libcgj_batch44298_marker[] = "libcgj-batch44298";

/* Soft milestone tag for wave 44300 (product identity = 44300). */
#define B44298_MILESTONE_TAG  44300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44298_tag(void)
{
	return B44298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_44300 - report soft milestone product tag for wave 44300.
 *
 * Always returns 44300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 44300.
 */
uint32_t
gj_milestone_tag_44300(void)
{
	(void)NULL;
	return b44298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_44300(void)
    __attribute__((alias("gj_milestone_tag_44300")));
