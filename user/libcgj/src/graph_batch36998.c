/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36998: milestone 37000 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_37000(void);
 *     - Returns the milestone tag for the milestone 37000 continuum
 *       (always 37000u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_37000  (alias)
 *   __libcgj_batch36998_marker = "libcgj-batch36998"
 *
 * Milestone 37000 exclusive continuum CREATE-ONLY (36991-37000). Unique
 * gj_milestone_tag_37000 surface only; no multi-def. Distinct from
 * gj_milestone_tag_36900 / gj_milestone_tag_36800 / gj_milestone_tag_36700,
 * gj_graph_milestone_37000 (batch37000), and gj_continuum_wave_37000
 * (batch36999).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (36991–37000): host soft probes
 * (cgj_soft_milestone_37000.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=37000 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36998. */
const char __libcgj_batch36998_marker[] = "libcgj-batch36998";

/* Soft milestone tag for wave 37000 (product identity = 37000). */
#define B36998_MILESTONE_TAG  37000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36998_tag(void)
{
	return B36998_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_37000 - report soft milestone product tag for wave 37000.
 *
 * Always returns 37000u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 37000.
 */
uint32_t
gj_milestone_tag_37000(void)
{
	(void)NULL;
	return b36998_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_37000(void)
    __attribute__((alias("gj_milestone_tag_37000")));
