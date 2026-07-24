/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch38798: milestone 38800 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_38800(void);
 *     - Returns the milestone tag for the milestone 38800 continuum
 *       (always 38800u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_38800  (alias)
 *   __libcgj_batch38798_marker = "libcgj-batch38798"
 *
 * Milestone 38800 exclusive continuum CREATE-ONLY (38791-38800). Unique
 * gj_milestone_tag_38800 surface only; no multi-def. Distinct from
 * gj_milestone_tag_38700 / gj_milestone_tag_38600 / gj_milestone_tag_38500,
 * gj_graph_milestone_38800 (batch38800), and gj_continuum_wave_38800
 * (batch38799).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (38791–38800): host soft probes
 * (cgj_soft_milestone_38800.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=38800 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 38798. */
const char __libcgj_batch38798_marker[] = "libcgj-batch38798";

/* Soft milestone tag for wave 38800 (product identity = 38800). */
#define B38798_MILESTONE_TAG  38800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b38798_tag(void)
{
	return B38798_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_38800 - report soft milestone product tag for wave 38800.
 *
 * Always returns 38800u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 38800.
 */
uint32_t
gj_milestone_tag_38800(void)
{
	(void)NULL;
	return b38798_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_38800(void)
    __attribute__((alias("gj_milestone_tag_38800")));
