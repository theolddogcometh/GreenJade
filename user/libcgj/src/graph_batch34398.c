/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34398: milestone 34400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_34400(void);
 *     - Returns the milestone tag for the milestone 34400 continuum
 *       (always 34400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_34400  (alias)
 *   __libcgj_batch34398_marker = "libcgj-batch34398"
 *
 * Milestone 34400 exclusive continuum CREATE-ONLY (34391-34400). Unique
 * gj_milestone_tag_34400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_34300 / gj_milestone_tag_34200 / gj_milestone_tag_34100,
 * gj_graph_milestone_34400 (batch34400), and gj_continuum_wave_34400
 * (batch34399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (34391–34400): host soft probes
 * (cgj_soft_milestone_34400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=34400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 34398. */
const char __libcgj_batch34398_marker[] = "libcgj-batch34398";

/* Soft milestone tag for wave 34400 (product identity = 34400). */
#define B34398_MILESTONE_TAG  34400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34398_tag(void)
{
	return B34398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_34400 - report soft milestone product tag for wave 34400.
 *
 * Always returns 34400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 34400.
 */
uint32_t
gj_milestone_tag_34400(void)
{
	(void)NULL;
	return b34398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_34400(void)
    __attribute__((alias("gj_milestone_tag_34400")));
