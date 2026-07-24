/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30398: milestone 30400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_30400(void);
 *     - Returns the milestone tag for the milestone 30400 continuum
 *       (always 30400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_30400  (alias)
 *   __libcgj_batch30398_marker = "libcgj-batch30398"
 *
 * Milestone 30400 exclusive continuum CREATE-ONLY (30391-30400). Unique
 * gj_milestone_tag_30400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_30300 / gj_milestone_tag_30200 / gj_milestone_tag_30100,
 * gj_graph_milestone_30400 (batch30400), and gj_continuum_wave_30400
 * (batch30399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30391–30400): host soft probes
 * (cgj_soft_milestone_30400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30398. */
const char __libcgj_batch30398_marker[] = "libcgj-batch30398";

/* Soft milestone tag for wave 30400 (product identity = 30400). */
#define B30398_MILESTONE_TAG  30400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30398_tag(void)
{
	return B30398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_30400 - report soft milestone product tag for wave 30400.
 *
 * Always returns 30400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 30400.
 */
uint32_t
gj_milestone_tag_30400(void)
{
	(void)NULL;
	return b30398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_30400(void)
    __attribute__((alias("gj_milestone_tag_30400")));
