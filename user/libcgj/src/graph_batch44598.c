/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44598: milestone 44600 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_44600(void);
 *     - Returns the milestone tag for the milestone 44600 continuum
 *       (always 44600u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_44600  (alias)
 *   __libcgj_batch44598_marker = "libcgj-batch44598"
 *
 * Milestone 44600 exclusive continuum CREATE-ONLY (44591-44600). Unique
 * gj_milestone_tag_44600 surface only; no multi-def. Distinct from
 * gj_milestone_tag_44500 / gj_milestone_tag_44400 / gj_milestone_tag_44300,
 * gj_graph_milestone_44600 (batch44600), and gj_continuum_wave_44600
 * (batch44599).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (44591–44600): host soft probes
 * (cgj_soft_milestone_44600.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=44600 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44598. */
const char __libcgj_batch44598_marker[] = "libcgj-batch44598";

/* Soft milestone tag for wave 44600 (product identity = 44600). */
#define B44598_MILESTONE_TAG  44600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44598_tag(void)
{
	return B44598_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_44600 - report soft milestone product tag for wave 44600.
 *
 * Always returns 44600u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 44600.
 */
uint32_t
gj_milestone_tag_44600(void)
{
	(void)NULL;
	return b44598_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_44600(void)
    __attribute__((alias("gj_milestone_tag_44600")));
