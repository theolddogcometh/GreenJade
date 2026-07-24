/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27298: milestone 27300 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_27300(void);
 *     - Returns the milestone tag for the milestone 27300 continuum
 *       (always 27300u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_27300  (alias)
 *   __libcgj_batch27298_marker = "libcgj-batch27298"
 *
 * Milestone 27300 exclusive continuum CREATE-ONLY (27291-27300). Unique
 * gj_milestone_tag_27300 surface only; no multi-def. Distinct from
 * gj_milestone_tag_27200 / gj_milestone_tag_27100 / gj_milestone_tag_27000,
 * gj_graph_milestone_27300 (batch27300), and gj_continuum_wave_27300
 * (batch27299).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (27291–27300): host soft probes
 * (cgj_soft_milestone_27300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=27300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27298. */
const char __libcgj_batch27298_marker[] = "libcgj-batch27298";

/* Soft milestone tag for wave 27300 (product identity = 27300). */
#define B27298_MILESTONE_TAG  27300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27298_tag(void)
{
	return B27298_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_27300 - report soft milestone product tag for wave 27300.
 *
 * Always returns 27300u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 27300.
 */
uint32_t
gj_milestone_tag_27300(void)
{
	(void)NULL;
	return b27298_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_27300(void)
    __attribute__((alias("gj_milestone_tag_27300")));
