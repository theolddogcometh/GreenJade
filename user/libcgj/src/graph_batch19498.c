/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch19498: milestone 19500 product tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_milestone_tag_19500(void);
 *     - Returns the milestone tag for the milestone 19500 continuum
 *       (always 19500u). Soft compile-time product identity tag.
 *   uint32_t __gj_milestone_tag_19500  (alias)
 *   __libcgj_batch19498_marker = "libcgj-batch19498"
 *
 * Milestone 19500 exclusive continuum CREATE-ONLY (19491-19500). Unique
 * gj_milestone_tag_19500 surface only; no multi-def. Distinct from
 * gj_milestone_tag_19400 / gj_milestone_tag_19300 / gj_milestone_tag_19200,
 * gj_graph_milestone_19500 (batch19500), and gj_continuum_wave_19500
 * (batch19499).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (19491–19500): host soft probes
 * (cgj_soft_milestone_19500.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=19500 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 19498. */
const char __libcgj_batch19498_marker[] = "libcgj-batch19498";

/* Soft milestone tag for wave 19500 (product identity = 19500). */
#define B19498_MILESTONE_TAG  19500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b19498_tag(void)
{
	return B19498_MILESTONE_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_milestone_tag_19500 - report soft milestone product tag for wave 19500.
 *
 * Always returns 19500u. Soft pure-data product identity tag. Does not
 * call libc. No parent wires.
 * CGJ soft KAT expectation: 19500.
 */
uint32_t
gj_milestone_tag_19500(void)
{
	(void)NULL;
	return b19498_tag();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_milestone_tag_19500(void)
    __attribute__((alias("gj_milestone_tag_19500")));
