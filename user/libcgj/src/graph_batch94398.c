/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94398: milestone 94400 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_94400(void);
 *     - Returns the milestone tag for the milestone 94400 continuum
 *       (always 94400u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_94400  (alias)
 *   __libcgj_batch94398_marker = "libcgj-batch94398"
 *
 * Milestone 94400 exclusive continuum CREATE-ONLY (25891-94400). Unique
 * gj_milestone_tag_94400 surface only; no multi-def. Distinct from
 * gj_milestone_tag_25800 / gj_milestone_tag_25700 / gj_milestone_tag_25600,
 * gj_graph_milestone_94400 (batch94400), and gj_continuum_wave_94400
 * (batch25899).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (25891–94400): host soft probes
 * (cgj_soft_milestone_94400.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=94400 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94398. */
const char __libcgj_batch94398_marker[] = "libcgj-batch94398";

/* Soft milestone tag for wave 94400 (product identity = 94400). */
#define B94398_MILESTONE_TAG  94400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94398_tag(void)
{
	return B94398_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_94400 - report soft milestone product tag for wave 94400.
 *
 * Always returns 94400u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 94400.
 */
uint32_t
gj_milestone_tag_94400(void)
{
	(void)NULL;
	return b94398_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_94400(void)
    __attribute__((alias("gj_milestone_tag_94400")));
