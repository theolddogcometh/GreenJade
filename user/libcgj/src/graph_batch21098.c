/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21098: milestone 21100 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_21100(void);
 *     - Returns the milestone tag for the milestone 21100 continuum
 *       (always 21100u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_21100  (alias)
 *   __libcgj_batch21098_marker = "libcgj-batch21098"
 *
 * Milestone 21100 exclusive continuum CREATE-ONLY (21091-21100). Unique
 * gj_milestone_tag_21100 surface only; no multi-def. Distinct from
 * gj_milestone_tag_21000 / gj_milestone_tag_20900 / gj_milestone_tag_20800,
 * gj_graph_milestone_21100 (batch21100), and gj_continuum_wave_21100
 * (batch21099).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (21091–21100): host soft probes
 * (cgj_soft_milestone_21100.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=21100 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21098. */
const char __libcgj_batch21098_marker[] = "libcgj-batch21098";

/* Soft milestone tag for wave 21100 (product identity = 21100). */
#define B21098_MILESTONE_TAG  21100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21098_tag(void)
{
	return B21098_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_21100 - report soft milestone product tag for wave 21100.
 *
 * Always returns 21100u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 21100.
 */
uint32_t
gj_milestone_tag_21100(void)
{
	(void)NULL;
	return b21098_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_21100(void)
    __attribute__((alias("gj_milestone_tag_21100")));
