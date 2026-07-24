/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63398: milestone 63400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_63400(void);
 *     - Returns the milestone tag for the milestone 63400 continuum
 *       (always 63400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_63400  (alias)
 *   __libcgj_batch63398_marker = "libcgj-batch63398"
 *
 * Milestone 63400 exclusive continuum CREATE-ONLY (25891-63400). Unique
 * gj_milestone_tag_63400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_63400 (batch63400), and gj_continuum_wave_63400
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–63400): host soft probes
 * (cgj_soft_milestone_63400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=63400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 63398. */
const char __libcgj_batch63398_marker[] = "libcgj-batch63398";

/* Soft milestone tag for wave 63400 (product identity = 63400). */
#define B63398_MILESTONE_TAG  63400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63398_tag(void)
{
	return B63398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_63400 - report soft milestone product tag for wave 63400.
 *
 * Always returns 63400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 63400.
 */
uint32_t
gj_milestone_tag_63400(void)
{
	(void)NULL;
	return b63398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_63400(void)
    __attribute__((alias("gj_milestone_tag_63400")));
