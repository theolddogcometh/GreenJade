/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47398: milestone 47400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_47400(void);
 *     - Returns the milestone tag for the milestone 47400 continuum
 *       (always 47400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_47400  (alias)
 *   __libcgj_batch47398_marker = "libcgj-batch47398"
 *
 * Milestone 47400 exclusive continuum CREATE-ONLY (47391-47400). Unique
 * gj_milestone_tag_47400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_47300 / gj_milestone_tag_47400 / gj_milestone_tag_47100,
 * gj_graph_milestone_47400 (batch47400), and gj_continuum_wave_47400
 * (batch47399).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (47391–47400): host soft probes
 * (cgj_soft_milestone_47400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=47400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 47398. */
const char __libcgj_batch47398_marker[] = "libcgj-batch47398";

/* Soft milestone tag for wave 47400 (product identity = 47400). */
#define B47398_MILESTONE_TAG  47400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b47398_tag(void)
{
	return B47398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_47400 - report soft milestone product tag for wave 47400.
 *
 * Always returns 47400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 47400.
 */
uint32_t
gj_milestone_tag_47400(void)
{
	(void)NULL;
	return b47398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_47400(void)
    __attribute__((alias("gj_milestone_tag_47400")));
